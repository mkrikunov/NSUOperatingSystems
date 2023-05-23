#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void checkUIDs() {
	printf("Real user ID : %d\n", getuid());
	printf("Effective user ID : %d\n", geteuid());

	FILE* fp = fopen("file.txt", "w");
	if (fp == NULL) {
		perror("fopen failed");
		exit(1);
	}
	else {
		fclose(fp);
	}
}



int main() {
	checkUIDs();
	printf("\n");
	int k = setuid(getuid());
	if (k == -1) {
		perror("setuid failed");
	}
	checkUIDs();
	return 0;
}
