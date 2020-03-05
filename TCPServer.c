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
    int sockfd_master, connfd, len, valread, i, sockfd, max_sd;
    int new_socket, client_socket[30], max_clients=30, activity; 
    struct sockaddr_in servaddr, cliaddr; 
    char *hello = "Hello from server";
    char buff[1024] = {0};//data buffer

    fd_set read_fd_set;

    //initialize client sockets to 0
    for(i=0; i<max_clients; i++) 
    	client_socket[i]=0;
  
    // socket create and verification 
    sockfd_master = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd_master == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd_master, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd_master, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    len = sizeof(cliaddr); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd_master, (SA*)&cliaddr, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 

    //valread = read (connfd, buff, 1024);
    //printf("%s\n", buff);
    send(connfd, hello, strlen(hello), 0);
    printf("Hello from server sent\n");

    while(1)
    {
    	//clear the set
    	FD_ZERO(&read_fd_set);

    	//add master socket to set
    	FD_SET(sockfd_master, &read_fd_set);
    	max_sd = sockfd_master;

    	//add child socket to set
    	for(i=0; i<max_clients; i++) {
    		sockfd = client_socket[i];

    		//check validity
    		if(sockfd > 0)
    			FD_SET(sockfd, &read_fd_set);

    		//highest fd number (for select ftn)
    		if(sockfd > max_sd)
    			max_sd = sockfd;
    	}

    	//wait for activity
    	activity = select(max_sd + 1, &read_fd_set, NULL, NULL, NULL);

    	if(activity < 0)
    		printf("Select function error\n");

    	/*If something happened to mamaster socket
    	then it is an incoming connection*/
    	if(FD_ISSET(sockfd_master, &read_fd_set)){
    		if ((new_socket = accept(sockfd_master, (SA*)&cliaddr, &len))<0) {
    			perror("accept");
    			exit(EXIT_FAILURE);
    		}

    		//send new connection hello message
    		if(send(new_socket, hello, strlen(hello), 0) != strlen(hello))
    			perror("send");

    		printf("Hello message sent succesfully\n");

    		//add new socket to array and listen to it's message
    		for(i=0; i<max_clients; i++) {
    			if(client_socket[i] == 0) {
    				client_socket[i] = new_socket;
    				printf("Adding to list of sockets as %d\n", i);
    				valread = read (client_socket[i], buff, 1024);
	    			//printf("%s\n", buff);
	    			char *ok = "Command ok";
	    			if(strcmp(buff, "USER")==0)
	    			{
	    				memset(buff, 0, sizeof(buff));
	    				valread=0;
	    				send(client_socket[i], ok, strlen(ok), 0);
	    				//valread = read (client_socket[i], buff, 1024);
	    				printf("Buffer is %s\n", buff);

	    			}
	    			else if(strcmp(buff, "PASS")==0)
	    			{
	    				memset(buff, 0, sizeof(buff));
	    				valread=0;
	    				send(client_socket[i], ok, strlen(ok), 0);
	    				//valread = read (client_socket[i], buff, 1024);
	    				printf("Buffer is %s\n", buff);

	    			}
    				break;
    			}
    		}
    	}
    }
  
    //Close the socket 
    close(sockfd); 
}