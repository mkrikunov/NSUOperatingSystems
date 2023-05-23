#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <aio.h>
#define BUFF_SIZE 512
#define NUM_OF_REQ 200
#define NUM_OF_CLIENTS 100




char* socket_path = "./socket";

int flag = 1;
int active_client = -1;
int free_clients = NUM_OF_CLIENTS;


typedef struct client {
    int client_num;
    char buff[BUFF_SIZE];
    struct aiocb block;
} client;

client clients[NUM_OF_CLIENTS];




void client_read(int num, int fd) {
    clients[num].client_num = num;
    clients[num].block.aio_fildes = fd;
    clients[num].block.aio_offset = 0;
    clients[num].block.aio_buf = clients[num].buff;
    clients[num].block.aio_nbytes = BUFF_SIZE;
    clients[num].block.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    clients[num].block.aio_sigevent.sigev_signo = SIGIO;
    clients[num].block.aio_sigevent.sigev_value.sival_int = num;
    aio_read(&clients[num].block);
}


void process(int num) {
    ssize_t size = aio_return(&clients[num].block);
    if (size == 0) {
	close(clients[num].block.aio_fildes);
	clients[num].client_num = 0;
	free_clients++;
   }
    else {
	for (ssize_t i = 0; i < size; i++) {
		clients[num].buff[i] = toupper(clients[num].buff[i]);
	}
	write(fileno(stdin), clients[num].buff, size);
	aio_read(&clients[num].block);
    }
}


int get_new_num() {
    for(int i = 1; i < NUM_OF_CLIENTS; i++) {
	if (clients[i].client_num == 0) {
    	    return i;
	}
    }
    return -1;
}


void sigio_catch(int sig, siginfo_t* info, void* context) {
    if (sig != SIGIO) {
	return;
    }

    int num = info->si_value.sival_int;
    if (aio_error(&clients[num].block) == 0) {
	active_client = num;
	process(active_client);
    }
}


void sigquit_catch(int sig) {
    flag = 0;
}



int main() {
    if (sigset(SIGQUIT, sigquit_catch) == SIG_ERR) {
	perror("signal catching failed");
	exit(1);
    }

    memset(clients, 0, NUM_OF_CLIENTS * sizeof(client));

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, socket_path);

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
	perror("socket failed");
	exit(1);
    }

    int k = bind(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (k == -1) {
	perror("bind failed");
	close(sockfd);
	exit(1);
    }

    int m = listen(sockfd, NUM_OF_REQ);
    if (m == -1) {
	perror("listen failed");
	unlink(sock_addr.sun_path);
	close(sockfd);
	exit(1);
    }


    sigset_t mysigset;
    sigemptyset(&mysigset);
    sigaddset(&mysigset, SIGIO);

    struct sigaction my_struct;
    sigemptyset(&my_struct.sa_mask);
    my_struct.sa_mask = mysigset;
    my_struct.sa_sigaction = sigio_catch;
    my_struct.sa_flags = SA_SIGINFO;
    sigaction(SIGIO, &my_struct, NULL);
    printf("\nThe server started:\n");

    while (flag) {
	int new_client_fd = accept(sockfd, NULL, NULL);
	if (new_client_fd == -1 && errno != EINTR) {
	    perror("accept failed");
	    continue;
	}
	if (new_client_fd == -1 && errno == EINTR) {
	    continue;
	}

	int new_client_num = get_new_num();
	if (new_client_num == -1) {
	    fprintf(stderr, "Too many clients. Please, wait and try again\n");
	}
	else {
	    free_clients--;
	    client_read(new_client_num, new_client_fd);
	}

    }
    unlink(socket_path);
    close(sockfd);
    return 0;
}
