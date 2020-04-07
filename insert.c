#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/ioctl.h>

#define MYFILE "./new.txt"

char title[]=
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

int main(int argc, char *argv[]){
	
	/* open file */
	FILE *fp = fopen(MYFILE, "a");
	
	if(!fp){
		perror("Open failed");
		return EXIT_FAILURE;
	}

	/* write to file */
	fputs(argv[1], fp);
	fputs("\n", fp);

	write(STDOUT_FILENO, title, sizeof(title) - 1);
	// output to stdout
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>INSERT</TITLE>\n");
    printf("<BODY><CENTER>Your insert: %s</CENTER>\n", argv[1]);
}