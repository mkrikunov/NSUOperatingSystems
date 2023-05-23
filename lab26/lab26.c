#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFSIZE 512

int main() {
    FILE* fptr = popen("tr [:lower:] [:upper:]", "w");
    if (fptr == NULL) {
	perror("popen failed");
	exit(1);
    }

    char buff[BUFFSIZE];
    int l;
    while((l = read(fileno(stdin), buff, BUFFSIZE)) > 0) {
	write(fileno(fptr), buff, l);
    }

    pclose(fptr);
    return 0;
}

