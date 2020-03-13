#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define delim " \t\r\n\a"
#define MAX 80 
#define PORT 8087
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
  
    //Socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
    	printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    //Assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    //Connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("Connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("Connected to the server..\n");

    

    send(sockfd, hello, strlen(hello), 0);
    printf("Hello from client sent\n");
    valread=read(sockfd, buff, 1024);
    printf("%s\n",buff );

    while(1){
    	printf("ftp> ");

    	//Read the input and store into str
    	char *inputstr = read_line();
    	printf("%s", inputstr);

	    if(*inputstr==NULL)
	      break;

	  	//tokenize the string
	  	/*const char *tokens[64];
	  	char *token=NULL;
	  	int i = 0;
	  	token = strtok(buff,delim);
	  	while (token != NULL){
	  	  tokens[i] = token;
	  	  i++;
	  	  if (i == 64)
	  	    tokens[i] ==NULL;
	  	  token = strtok(NULL, delim);
	  	}
	  	tokens[i]=NULL;*/

	  	memset(buff, 0, sizeof(buff));
	  	send(sockfd, inputstr, strlen(inputstr), 0);
	  	send(sockfd, inputstr, strlen(inputstr), 0);
	  	if(inputstr[0] == 'U' && inputstr[1] == 'S' && inputstr[2] == 'E' && inputstr[3] == 'R') {
	   		send(sockfd, inputstr, strlen(inputstr), 0);
	  		printf("Input sent to server\n");
	  		valread=read(sockfd, buff, 1024);
	  		printf("%s\n",buff );
	  	}
	  	if(inputstr[0] == 'P' && inputstr[1] == 'A' && inputstr[2] == 'S' && inputstr[3] == 'S') {
	  		send(sockfd, inputstr, strlen(inputstr), 0);
	  		printf("Input sent to server\n");
	  		valread=read(sockfd, buff, 1024);
	  		printf("%s\n",buff );
	  	}
	    
	   if(inputstr[0] == 'P' && inputstr[1] == 'U' && inputstr[2] == 'T') {
		   	send(sockfd, inputstr, strlen(inputstr), 0);
		   	printf("Input sent to server\n");
		   	valread=read(sockfd, buff, 1024);
		   	printf("%s\n",buff );
	   }
	}  
    //Close the socket 
    close(sockfd); 
} 
