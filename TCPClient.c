#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>
#define delim " \t\r\n\a"
#define MAX 80 
#define PORT 8093
#define SA struct sockaddr 
#define BUFSIZE 1024

/* Read input */
char *read_line(void){
    char *buffer=malloc(BUFSIZE*sizeof(char));
    memset(buffer, 0, sizeof(buffer));
    size_t bufsize = 0;
    getline(&buffer, &bufsize, stdin);
    return buffer;
}

/*Tokenizing the string to seperate the command name from input*/
char** tokenizeString(char *input) {
    if(input == NULL)
        return NULL;

    char **tokens = malloc(BUFSIZE*sizeof(char*));
    char *token = malloc(BUFSIZE*sizeof(char*));
    int i = 0;
    strcpy(token, input); //need the original input to send to server
    token = strtok(token,delim);
    while (token != NULL){
        tokens[i] = token;
        i++;
        if (i == 64)
            tokens[i] ==NULL;
        token = strtok(NULL, delim);
    }
    tokens[i]=NULL;
    return tokens;

}

/*void put(int socket, char *filename) {
	struct stat obj;
	stat(filename, &obj);
	size_t size = obj.st_size;
	int file;
	if((file = open(filename, O_RDONLY))<0) {
		perror("File opening");
		size = 0;
		send(socket, &size, sizeof(size_t), 0);
		return;
	}
	send(socket, &size, sizeof(size_t), 0);
	printf("sent1\n");
	sendfile(socket, file, NULL, size);
	printf("sent2\n");
	close(file);
	return;


}*/

void get(int socket, char *filename) {
	size_t instr=0;

	if(read(socket, &instr, sizeof(size_t))<0) {
        perror("First read");
        return;
    }
    if(!instr) {
        printf("Path or directory doesn't exist\n");
        return;
    }

    int file;
    if((file = open(filename, O_CREAT| O_WRONLY, 0666))<0) {
        perror("Opening");
        return;
    }

    char *hold = malloc(instr);
    size_t r = 0;
    void *buf;
    size_t w;
    while(0<instr) {
        r+= read(socket, hold, instr);
        buf = hold;
        if(r<0) {
            perror("Second read");
            free(hold);
            return;
        }
        while(r>0) {
            w = write(file, buf, r);
            instr-=w;
            r-=w;
            buf+=w;
        }
    }
    printf("DONE\n");
    free(hold);
    close(file);
    return;

}
  
int main() 
{
    int sockfd, connfd, valread; 
    struct sockaddr_in servaddr, cli; 

    char *hello="Hello from client";
    char *buff=malloc(BUFSIZE*sizeof(char));
    char *inputstr=malloc(BUFSIZE*sizeof(char));
  
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

    

    while(1){
    	printf("ftp> ");

    	//Read the input and store into str
    	inputstr = read_line();
    	//printf("%s", inputstr);

	    if(inputstr==NULL)
	      break;

	  	memset(buff, 0, sizeof(buff));

	  	//If there is ! skip it and leave it to server to decide
	  	if(!strncmp(inputstr, "!", 1)) {
	  		inputstr++;
	  	}

	  	char **args=malloc(BUFSIZE*sizeof(char));
	  	if((args=tokenizeString(inputstr))==NULL) {
	  		printf("No input\n");
	  		continue;
	  	}
	  	//printf("%s\n", inputstr);

	  	if(strcmp(args[0], "USER")==0) {
	   		send(sockfd, inputstr, strlen(inputstr), 0);
	  		//printf("Input sent to server\n");
	  		valread=read(sockfd, buff, 1024);
	  		printf("%s\n",buff );
	  	}
	  	if(strcmp(args[0], "PASS")==0) {
	  		send(sockfd, inputstr, strlen(inputstr), 0);
	  		//printf("Input sent to server\n");
	  		valread=read(sockfd, buff, 1024);
	  		printf("%s\n",buff );
	  	}
	    
	   if(strcmp(args[0], "PWD")==0) {
		   	send(sockfd, inputstr, strlen(inputstr), 0);
		   	//printf("Input sent to server\n");
		   	valread=read(sockfd, buff, 1024);
		   	printf("%s\n",buff );
	   	}
		if(strcmp(args[0], "LS")==0) {
	   	   	send(sockfd, inputstr, strlen(inputstr), 0);
	   	   	//printf("Input sent to server\n");
	   	   	valread=read(sockfd, buff, 1024);
	   	   	printf("%s\n",buff );
	    }
	    if(strcmp(args[0], "CD")==0) {
	   	   	send(sockfd, inputstr, strlen(inputstr), 0);
	   	   	//printf("Input sent to server\n");
	   	   	valread=read(sockfd, buff, 1024);
	   	   	printf("Now you're in dir: %s\n",buff );
	    }
	    if(strcmp(args[0], "PUT")==0) {
		   	send(sockfd, inputstr, strlen(inputstr), 0);
		   	//printf("%s\n",args[1] );
		   	//put(sockfd, args[1]);
		   	//printf("Input sent to server\n");
		   	//valread=read(sockfd, buff, 1024);
		   	//printf("%s\n",buff );
	   	}
	   	if(strcmp(args[0], "GET")==0) {
		   	send(sockfd, inputstr, strlen(inputstr), 0);
			get(sockfd, args[1]);
		}
	}  
    //Close the socket 
    close(sockfd); 
} 



