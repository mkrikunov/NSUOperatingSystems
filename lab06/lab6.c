#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#define BUFF_SIZE 512


typedef struct line {
    off_t pos;
    off_t length;
} Line;

typedef struct vector {
    int cnt;
    int cap;
    Line* ptr;
} Vector;

Vector table;



void add_line(Vector* table, Line line) {
    if (table->cnt >= table->cap) {
	if(table->cap == 0) {
	    table->ptr = (Line*)malloc(5 * sizeof(Line));
	    table->cap = 5;
	}
	else {
	    table->cap *= 2;
	    table->ptr = (Line*)realloc(table->ptr, table->cap * sizeof(Line));
	}
    }
    table->ptr[table->cnt] = line;
    table->cnt++;
}


void make_a_table(int fd, Vector* table) {
    char  buff[BUFF_SIZE];
    off_t pos = 0;
    off_t seek = 0;
    off_t length = 0;

    int feof = 0;

    while(feof == 0) {
	int num = read(fd, buff, BUFF_SIZE);

	if (num == 0) {
	    feof = 1;
	}

	Line new_line;
	for(int i = 0; i < num; i++) {
	    if (buff[i] == '\n') {
		new_line.pos = pos;
		new_line.length = length;
		add_line(table, new_line);

		length = 0;
		pos = seek + 1;
		seek++;
		continue;
	    }
	    seek++;
	    length++;
	}
    }
}



int main(int argc, char** argv){

    char* path = argv[1];

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
	perror("open failed");
	exit(1);
    }

    make_a_table(fd, &table);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    fd_set readfds;

    setvbuf(stdin, NULL, _IONBF, 0);
    printf("Enter the line number within 5 seconds:\n");
    while(1) {
	int line_number;
	FD_ZERO(&readfds);
	FD_SET(fileno(stdin), &readfds);

	int k = select(fileno(stdin) + 1, &readfds, NULL, NULL, &timeout);
	if (k == -1) {
	    perror("select failed");
	    close(fd);
	    exit(1);
	}
	if (k == 0) {
	    write(fileno(stdout), "\nTimes up!\n", 11);
	    lseek(fd, 0, SEEK_SET);
	    char buff[BUFF_SIZE];
	    int bytes_num;
	    while((bytes_num = read(fd, buff, BUFF_SIZE)) > 0) {
		write(fileno(stdout), buff, bytes_num);
	    }
	    exit(0);
	}

	int m = fscanf(stdin, "%d", &line_number);
	if (feof(stdin)) {
	    fprintf(stderr, "EOF occured\n");
	    break;
	}
	if (m == 0) {
	    scanf("%*[^\n]");
	    fprintf(stderr, "Invalid input data. Please, try again\n");
	    continue;
	}
	if (m == -1) {
	    perror("scanf failed");
	    free(table.ptr);
	    close(fd);
	    exit(1);
	}


	if (line_number < 0) {
	    printf("The line number must be positive!\n");
	}
	else if (line_number == 0) {
	    scanf("%*[^\n]");
	    printf("\nGoodbye!\n");
	    break;
	}
	else if (line_number > table.cnt) {
	    printf("There are only %d lines in the file\n", table.cnt);
	}
	else {
	    Line ans = table.ptr[line_number - 1];
	    lseek(fd, ans.pos, SEEK_SET);

	    char buff[BUFF_SIZE];
	    int bytes_num;
	    while(ans.length > 0) {
	        if (ans.length < BUFF_SIZE) {
		    bytes_num = read(fd, buff, ans.length);
		}
		else {
		    bytes_num = read(fd, buff, BUFF_SIZE);
		}
		ans.length -= bytes_num;
		write(fileno(stdout), buff, bytes_num);
	    }
	    printf("\n");
	}
    }

    free(table.ptr);
    close(fd);
    return 0;
}
