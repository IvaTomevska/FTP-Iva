#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/select.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr


int main() 
{ 
    int sockfd_master, connfd, len, i, max_fd;
    int new_socket, activity; 
    struct sockaddr_in servaddr, cliaddr; 
    char *hello = "Hello from server";
    char buff[1024];//data buffer

    fd_set read_fd_set; //temp file desctiptor list for select()
    fd_set sockfd_master_set; //master file descriptor list

    //Slear the sets
    FD_ZERO(&read_fd_set);
    FD_ZERO(&sockfd_master_set);
  
    //Socket create and verification 
    sockfd_master = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd_master == -1) { 
        printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    
    bzero(&servaddr, sizeof(servaddr)); 
  
    //Assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    //Binding newly created socket to given IP and verification 
    if ((bind(sockfd_master, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    //Now server is ready to listen and verification 
    if ((listen(sockfd_master, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    len = sizeof(cliaddr); 
  
    //Accept the data packet from client and verification 
    connfd = accept(sockfd_master, (SA*)&cliaddr, &len); 
    if (connfd < 0) { 
        printf("Server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server acccept the client...\n");

    //Add master socket to set
    FD_SET(sockfd_master, &sockfd_master_set);
    max_fd = sockfd_master; 


    send(connfd, hello, strlen(hello), 0);
    printf("Hello from server sent\n");

    while(1)
    {
        read_fd_set = sockfd_master_set;
    	
    	//Wait for activity
    	activity = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);

    	if(activity < 0)
    		printf("Select function error\n");

        //Run through the existing connections looking for data to read
        for(i=0; i <= max_fd; i++) {
            if(FD_ISSET(i, &read_fd_set)) {
                if(i == sockfd_master) {
                    //Handle new connections
                    len = sizeof(cliaddr);

                    if((new_socket = accept(sockfd_master, (SA*)&cliaddr, &len))<0) {
                        perror("Accept function error\n");
                        exit(EXIT_FAILURE);
                    }
                    else {
                        FD_SET(new_socket, &sockfd_master_set);

                        //Keep track of highest fd number
                        if(new_socket > max_fd)
                            max_fd = new_socket;

                        printf("New connection from %d on socket %d\n", inet_ntoa(cliaddr.sin_addr), new_socket);
                    }
                }
            }
        }	
    }
  
    //Close the socket 
    close(sockfd_master); 
}