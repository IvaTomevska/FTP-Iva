#include <netdb.h> 
#include <stdio.h> /* for printf and fprintf */
#include <stdlib.h> /* for atoi */
#include <string.h> /* for memset */
#include <sys/socket.h> /* for socket, connect, send, and recv */
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

  
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

    send(sockfd, hello, strlen(hello), 0);
    printf("Hello from client sent\n");
    valread=read(sockfd, buff, 1024);
    printf("%s\n",buff );
  
    // close the socket 
    close(sockfd); 
} 
