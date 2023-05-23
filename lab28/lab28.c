#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>

#define MAX 100

int main() {
    FILE * fptrs[2];

    srand(time(NULL));

    pid_t pid = p2open("/bin/sort -n", fptrs);
    if (pid == -1) {
        perror("Unsuccessful execution of p2open");
        exit(1);
    }

    for(int i = 0; i < MAX; i++) {
        int n = rand() % 100;
        fprintf(fptrs[0], "%d\n", n);
    }
    fclose(fptrs[0]);

    for (int i = 0; i < MAX; i++) {
        int num;

        int t = fscanf(fptrs[1], "%d", &num);

        if (feof(fptrs[1])) break;
        if (t == 0) {
            fprintf(stderr, "invalid input data");
            break;
        }
        if (t == -1) {
            perror("Unsuccessful execution of fscanf");
            pclose(fptrs[1]);
            exit(1);
        }

	printf("%2d ", num);

        if ((i + 1) % 10 == 0) {
            printf("\n");
        }

    }

    if (pclose(fptrs[1]) == -1) {
        perror("Unsuccessful execution of pclose");
        exit(1);
    }

    return 0;
}
