#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#define BUFF_SIZE 100

char* socket_path = "./socket";

int main(){
    struct sockaddr_un addr;
    char buff[BUFF_SIZE] = {0};

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)  {
	perror("socket failed");
	exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    int k = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (k == -1) {
	perror("connect failed");
	exit(1);
    }

    ssize_t n;
    while((n = read(fileno(stdin), buff, BUFF_SIZE)) > 0) {
	if (write(sockfd, buff, n) == -1) {
	    perror("write failed");
	    exit(1);
	}
    }
    close(sockfd);
    return 0;
}
