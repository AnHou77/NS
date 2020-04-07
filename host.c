#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#define port 8666

char myWeb[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>myServer</title></head>\r\n"
"<body><center><h1><font color=\"red\" />Welcome to my server</font></h1><br>\r\n"
"<form action='insert.cgi' method='GET'>"
"INSERT：<input type='text' name='data' />"
"<input type='submit' value='GET' />"
"<input type='submit' formmethod='post' value='POST'>"
"</form>"
"<form action='view.cgi' method='GET'>"
"<input type='submit' value='VIEW'>"
"</form>"
"</center></body></html>\r\n";

int main(int argc, char *argv[])
{
	int cgiInput[2];
	int cgiOutput[2];
	int status;
	pid_t pid;
	char c;
	char *inputData = "HELLO WORLD";


	/* the address of server & client */
	struct sockaddr_in server_addr, client_addr;
	/* client's address size */
	socklen_t sin_len = sizeof(client_addr);
	/* file descriptor of server and client */
	int fd_server, fd_client;
	/* stores the content from the browser to server */
	char buff[2048];
	
	int on = 1;

	/* 設定socket類型 取得file descriptor*/
	fd_server = socket(AF_INET,SOCK_STREAM,0);

	/* socket() 錯誤會return -1 需要作error handling */
	if(fd_server < 0){
		perror("socket");
		exit(1);
	}

	/* 設定socket（fd_server）其他選項 */
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	/* 設定server address */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	/* bind */
	if(bind(fd_server, (struct  sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("bind");
		close(fd_server);
		exit(1);
	}

	/* listen */
	if(listen(fd_server, 10) == -1){
		perror("listen");
		close(fd_server);
		exit(1);
	}

	while(1){
		
		/* pipe 兩通道 host -> cgi & cgi -> host*/
		if(pipe(cgiInput)<0){
			perror("pipe");
			exit(EXIT_FAILURE);
		}

		if(pipe(cgiOutput)<0){
			perror("pipe");
			exit(EXIT_FAILURE);
		}

		/* 用accept建立server與client的連結 */
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);
		
		if(fd_client == -1){
			perror("Connection failed...\n\n");
			continue;
		}

		printf("======= Connect with client =======\n\n");

		/* fork新的process來處理cgi */
		if((pid = fork()) < 0){
			perror("fork");
			exit(EXIT_FAILURE);
		}

		/* child process */
		if(pid == 0){

			printf("----- Child process -----\n\n");
			
			/* 關掉沒用到的fd */
			close(cgiInput[1]);
			close(cgiOutput[0]);

			/* child不需要也關掉 */
			close(fd_server);
			
			/* 取出fd的資訊到buffer */
			memset(buff, 0, 2048);
			read(fd_client, buff, 2047);
			printf("----- Get request -----\n");
			printf("\n%s", buff);
			printf("\n-----------------------\n\n");

			/*redirect input & output to CGI input,output*/
			dup2(cgiOutput[1],STDOUT_FILENO);
			dup2(cgiInput[0],STDIN_FILENO);

			/* redirect後用不到也關掉 */
			close(cgiInput[0]);
			close(cgiOutput[1]);
			
			close(fd_client);

			/* 分析request */
			if(strncmp(buff, "GET /view.cgi", 13) == 0){
				execlp("./view.cgi","./view.cgi",NULL);
			}
			else if(strncmp(buff, "GET /insert.cgi?data=", 21) == 0){
				inputData = strtok(buff," ");
				inputData = strtok(NULL," ");
				/* 取 /insert.cgi?data= 後的值 */
				inputData = inputData + 17;
				/* 將要插入的值 丟到cgi裡 */
				execlp("./insert.cgi","./insert.cgi", inputData, NULL);
			}
			else if(strncmp(buff, "POST /insert.cgi", 16) == 0){
				char* str = "data=";
				int loc = strstr(buff,str) - buff;
				inputData = buff + (loc + 5);
				execlp("./insert.cgi","./insert.cgi", inputData, NULL);
			}
			else if(strncmp(buff, "GET / ", 6) == 0){
				write(STDOUT_FILENO, myWeb, sizeof(myWeb) - 1);
			}
			else{
				execlp("./404.cgi","./404.cgi",NULL);
			}
			exit(0);
		}
		/* parent process*/
		else{
			
			printf("----- Parent process -----\n\n");
			
			close(cgiOutput[1]);
			close(cgiInput[0]);

			/* 從host傳送訊息到CGI 實作方法沒用到因此註解掉 */
			//write(cgiInput[1], inputData, strlen(inputData));
			
			/* 接收CGI傳來的訊息 */
			while(read(cgiOutput[0], &c, 1) > 0){
				write(fd_client, &c, 1);
			}

			close(cgiOutput[0]);
			close(cgiInput[1]);
			
			waitpid(pid, &status, 0);
			
			close(fd_client);
		}
	}
	return 0;
}
