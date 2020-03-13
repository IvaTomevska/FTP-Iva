#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <pwd.h>
#define MAX 80 
#define PORT 8087
#define SA struct sockaddr
#define delim " \t\r\n\a"


int main() 
{ 
    int sockfd_master, len, i, max_fd, sockfd, check;
    int new_socket, activity, valread, client_socket[30], max_clients=30; 
    struct sockaddr_in servaddr, cliaddr; 
    struct passwd pwd; //*check;
    char *hello = "Hello from server";
    char buff[1024];//data buffer

    fd_set read_fd_set; //file desctiptor list

    //Initialize client sockets to 0
    for(i=0; i<max_clients; i++) 
        client_socket[i]=0;

    //Slear the sets
    FD_ZERO(&read_fd_set);
  
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


    send(connfd, hello, strlen(hello), 0);
    printf("Hello from server sent\n");
    while(1) {
        FD_ZERO(&read_fd_set);
        printf("mmm\n");

        //Add master socket to set
        FD_SET(sockfd_master, &read_fd_set);
        max_fd = sockfd_master;
        printf("%d",max_fd);

        //Add child socket to set
        for(i=0; i<max_clients; i++) {
            printf("%d\n", i);
            sockfd = client_socket[i];

            //Check validity
            if(sockfd > 0)
            {
            printf("tatko ti e zena\n");
            FD_SET(sockfd, &read_fd_set);
                }
            //Highest fd number (for select ftn)
            if(sockfd > max_fd)
                max_fd = sockfd;
        }
    	printf("%d\n", max_fd);
    	//Wait for activity
    	activity = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);
        
        if(activity < 0)
            {printf("Select function error\n");
                        continue;}

        /*If something happened to mamaster socket
        then it is an incoming connection*/
        if(FD_ISSET(sockfd_master, &read_fd_set)){

            if ((new_socket = accept(sockfd_master, (SA*)&cliaddr, &len))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("Accepted connection\n");
            // Now server is ready to listen and verification 
            if ((listen(sockfd_master, 5)) != 0) { 
                printf("Listen failed...\n"); 
                exit(0); 
            } 
            printf("Server listening..\n"); 
            

            //send new connection hello message
            if(send(new_socket, hello, strlen(hello), 0) != strlen(hello))
                perror("send");

            printf("Hello message sent succesfully\n");

            //add new socket to array and listen to it's message
            for(i=0; i<max_clients; i++) {
                if(client_socket[i] == 0) {
                    
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

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

        for(i=0; i<max_clients; i++) {
            memset(buff, 0, sizeof(buff));
            sockfd = client_socket[i];
            if(FD_ISSET(sockfd, &read_fd_set)) {
                if (valread = read (sockfd, buff, 1024)==0) {
                    close(sockfd);
                    client_socket[i]=0;
                    printf("Client disconnected\n");
                }
                else if(buff[0] == 'U' && buff[1] == 'S' && buff[2] == 'E' && buff[3] == 'R')
                {
                    char *msg;

                    msg = "Username OK, Password required";
                    send(client_socket[i], msg, strlen(msg), 0);
                    printf("Checking user\n");

                }
                else if(buff[0] == 'P' && buff[1] == 'A' && buff[2] == 'S' && buff[3] == 'S')
                {
                    char *msg;

                    memset(buff, 0, sizeof(buff));
                    valread=0;
                    msg = "Authentication complete";
                    send(client_socket[i], msg, strlen(msg), 0);

                }
                 else if(buff[0] == 'P' && buff[1] == 'U' && buff[2] == 'T')
                {
                    char *msg;

                    memset(buff, 0, sizeof(buff));
                    valread=0;
                    msg = "Authentication complete";
                    send(client_socket[i], msg, strlen(msg), 0);

                }
                break;
            }       
        }
    }
    
    //Close the socket 
    close(sockfd_master); 
}