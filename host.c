#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

char webpage[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>myServer</title></head>\r\n"
"<body><center><h3>Welcome to my server</h3><br>\r\n";

int main(int argc, char *argv[])
{
	/* the address of server & client */
	struct sockaddr_in server_addr, client_addr;
	/* client's address size */
	socklen_t sin_len = sizeof(client_addr);
	/* file descriptor of server and client */
	int fd_server, fd_client;
	/* stores the content from the browser to server */
	char buff[2048];
	int fdimg;
	int on = 1;

	fd_server = socket(AF_INET,SOCK_STREAM,0);

	if(fd_server < 0){
		perror("socket");
		exit(1);
	}

	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8888);

	if(bind(fd_server, (struct  sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server, 10) == -1){
		perror("listen");
		close(fd_server);
		exit(1);
	}

	while(1){
		
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

		printf("%d\n",fd_client);
		if(fd_client == -1){
			perror("Connection failed...\n");
			continue;
		}

		printf("Got client connection...\n");

		if(!fork()){

			/* child process */
			close(fd_server);
			memset(buff, 0, 2048);
			read(fd_client, buff, 2047);

			printf("%s\n", buff);

			if(!strncmp(buff, "GET /index.jpeg", 15)){
				fdimg = open("index.jpeg", O_RDONLY);
				sendfile(fd_client, fdimg, NULL, 8000);
				close(fdimg);
			}
			else
				write(fd_client, webpage, sizeof(webpage) - 1);
		}

		/* parent process*/
		close(fd_client);
		printf("closing...\n");
		exit(0);
	}

	return 0;
}
