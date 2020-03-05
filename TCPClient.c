#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define delim " \t\r\n\a"
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

/* Read input */
char *read_line(void){
    char *buffer = NULL;
    size_t bufsize = 0;
    getline(&buffer, &bufsize, stdin);
    return buffer;
}

  
int main() 
{
    int sockfd, connfd, valread; 
    struct sockaddr_in servaddr, cli; 

    char *hello="Hello from client";
    char buff[1024]={0};
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n");

    while(1){
    	printf("ftp> ");

    	//Read the input and store into str
		char *inputstr = read_line();
		printf("%s", inputstr);

		if(*inputstr==NULL){
			break;
		}

		//tokenize the string
		char **tokens[64];
		char *token=NULL;
		int i = 0;
		token = strtok(inputstr,delim);
		while (token != NULL){
			tokens[i] = token;
			i++;
			if (i == 64)
				tokens[i] ==NULL;
			token = strtok(NULL, delim);
		}
		tokens[i]=NULL;

		if(strcmp(tokens[0],"USER")==0){
			send(sockfd, tokens[0], strlen(tokens[0]), 0);
			printf("Command sent to server\n");
			valread=read(sockfd, buff, 1024);
			if(strcmp(buff,"Command ok")){
				send(sockfd, tokens[1], strlen(tokens[1]), 0);
				printf("SERVER SAID OK\n");
			}
			//send(sockfd, tokens[1], strlen(tokens[1]), 0);
			/*printf("%s\n", tokens[0]);
			printf("%s\n", tokens[1]);
			strcat(tokens[0], tokens[1]);
			printf("%s\n", tokens[0]);*/

		}
		else if(strcmp(tokens[0],"PASS")==0){
			send(sockfd, tokens[0], strlen(tokens[0]), 0);
			printf("Command sent to server\n");
			valread=read(sockfd, buff, 1024);
			if(strcmp(buff,"Command ok")){
				send(sockfd, tokens[1], strlen(tokens[1]), 0);
				printf("SERVER SAID OK\n");
			}
		}


    } 

    send(sockfd, hello, strlen(hello), 0);
    printf("Hello from client sent\n");
    valread=read(sockfd, buff, 1024);
    printf("%s\n",buff );
  
    // close the socket 
    close(sockfd); 
} 
