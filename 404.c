#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/ioctl.h>

#define MYFILE "./new.txt"

char title[]=
"HTTP/1.1 404 Not Found\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

int main(void){
	write(STDOUT_FILENO, title, sizeof(title) - 1);
	// output to stdout
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>404 ERROR</TITLE>\n");
    printf("<BODY><CENTER><b>Oops! Something wrong!</b></CENTER>\n");
}