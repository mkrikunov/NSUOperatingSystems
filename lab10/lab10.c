#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
	fprintf(stderr, "There must be more than one argument\n");
	exit(1);
    }

    pid_t pid = fork();
    if (pid > 0) {
	//parent
	int status;
	wait(&status);
	if (WIFEXITED(status)) {
	    printf("The exit code of the child process is %d\n", WEXITSTATUS(status));
	}
    }
    else if (pid == 0) {
	//child
	execvp(argv[1], &argv[1]);
	perror("execvp failed");
	exit(1);
    }
    else {
	perror("fork failed");
	exit(1);
    }

    return 0;
}
