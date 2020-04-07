#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/ioctl.h>

#define MYFILE "./new.txt"

char title[]=
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

int main(void){

	FILE *fp = fopen(MYFILE,"r");

	if(!fp) {
		perror("Open failed");
		return EXIT_FAILURE;
	}

	char c;
	while((c = fgetc(fp)) != EOF){
		putchar(c);
	}
	
	fclose(fp);

	write(STDOUT_FILENO, title, sizeof(title) - 1);
	// output to stdout
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>VIEW</TITLE>\n");
}