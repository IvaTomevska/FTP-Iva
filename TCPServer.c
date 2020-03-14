#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#define MAX_CLIENTS 30 
#define PORT 8092
#define SA struct sockaddr
#define delim " \t\r\n\a"
#define BUFSIZE 1024
char ** users;
char ** pws;
int usersNum = 0;

struct user
{
    int user;
    int pass;
    char *currDir;//we need the directory for multiple finctions

};

struct user userList[MAX_CLIENTS];

int main() 
{ 
    int sockfd_master, len, i, max_fd, sockfd, check;
    int new_socket, activity, valread, client_socket[MAX_CLIENTS];
    memset(client_socket, 0, sizeof(client_socket));
    struct sockaddr_in servaddr, cliaddr; 
    char *buff=malloc(BUFSIZE*sizeof(char));//data buffer

    if(createAcc() == -1) {
        printf("Account creation fail\n");
        exit(EXIT_FAILURE);
    }

    fd_set read_fd_set; //file desctiptor list

    //Slear the sets
    FD_ZERO(&read_fd_set);
  
    //Socket create and verification 
    sockfd_master = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd_master == -1) { 
        printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    //else
    //printf("Socket successfully created..\n"); 
    
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
  
    //Now server is ready to listen and verification 
    if ((listen(sockfd_master, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    //else
    printf("Server listening and waiting for connection..\n"); 
    
    len = sizeof(cliaddr); 

    for (i=0; i<MAX_CLIENTS; i++) {
        userList[i].user = -1;
        userList[i].pass = 0;
        userList[i].currDir = malloc(BUFSIZE*sizeof(char));
        //printf("EDEN\n");
    }

    while(1) {
        memset(buff, 0, sizeof(buff));

        FD_ZERO(&read_fd_set);

        //Add master socket to set
        FD_SET(sockfd_master, &read_fd_set);
        max_fd = sockfd_master;

        //Add child socket to set
        for(i=0; i<MAX_CLIENTS; i++) {
            sockfd = client_socket[i];

            //Check validity
            if(sockfd > 0)
                FD_SET(sockfd, &read_fd_set);
                
            //Highest fd number (for select ftn)
            if(sockfd > max_fd)
                max_fd = sockfd;
        }

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
            //printf("Accepted connection\n");
            // Now server is ready to listen and verification 
            if ((listen(sockfd_master, 5)) != 0) { 
                printf("Listen failed...\n"); 
                exit(0); 
            } 

            //Add new socket to array
            for(i=0; i<MAX_CLIENTS; i++) {
                if(client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    getwd(buff);
                    strcpy(userList[i].currDir, (char*)buff);
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }
        //Check input from clients
        for(i=0; i<MAX_CLIENTS; i++) {
            memset(buff, 0, sizeof(buff));
            sockfd = client_socket[i];
            if(FD_ISSET(sockfd, &read_fd_set)) {
                if (valread = read (sockfd, buff, 1024)==0) {
                    close(sockfd);
                    client_socket[i]=0;
                    printf("Client disconnected\n");
                }
                else {
                    buff[valread-1] = '\0';//NULL terminating buffer
                    char *temp = malloc (strlen(buff)*sizeof(char));
                    strcpy(temp, buff);
                    //printf("%s\n", buff);
                    execute(temp, sockfd, i);
                    free(temp);
                }
                break;
            }       
        }
    }
    
    //Close the socket 
    close(sockfd_master); 
}

/*Primary function that stores users and passwords*/
int createAcc(void) {
    printf("Enter number of users: ");
    char *temp = malloc(BUFSIZE*sizeof(char));
    size_t size = 0;
    int input = getline(&temp, &size, stdin);
    input--;
    //check if it is smaller that BUFSIZE of empty
    if(input >= BUFSIZE) {
        printf("sex1\n");
        free(temp);
        return -1;
    }
    if(input == 0) {
        free(temp);
        return -1;
    }
    for(int i=0; i<input; i++) {
        if(!isdigit(temp[i])) {
            printf("Invalid number\n");
            free(temp);
            return -1;
        }
    }

    int usernum=atoi(temp);;
    users = malloc(usernum*sizeof(char*));
    pws = malloc(usernum*sizeof(char*));
    for (int i=0; i < usernum; i++) {
        printf("Enter username: ");
        input = getline(&temp, &size, stdin);
        if(input >= BUFSIZE) {
            printf("Input too long\n");
            free(temp);
            free(users);
            free(pws);
            return -1;
        }
        /*Getline has \n in the end,
        This is so it doesn't mess up the comparison*/
        temp[strlen(temp)-1] = '\0';
        strcpy(users+i, temp);

        printf("Enter password for %s: ", users+i);
        input = getline(&temp, &size, stdin);
        if(input >= BUFSIZE) {
            printf("Input too long\n");
            free(temp);
            free(users);
            free(pws);
            return -1;
        }
        temp[strlen(temp)-1] = '\0';
        strcpy(pws+i, temp);
    }

        usersNum = usernum;
        users[usernum] = NULL;
        pws[usernum] = NULL;
        return 0;


}

/*Tokenizing the string to seperate the command name from input*/
char** tokenizeString(char *input) {
    if(input == NULL)
        return NULL;

    char **tokens = malloc(BUFSIZE*sizeof(char*));
    char *token = malloc(BUFSIZE*sizeof(char*));
    int i = 0;
    token = strtok(input,delim);
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

/*Searching for the user by username 
and sending message to client*/
void findUser(char *input, int socket, int user) {
    for(int i=0; i<usersNum; i++) {
        if(!strcmp(input, users+i)){
            printf("Yes sex8\n");
            userList[user].user = i;
            send(socket, "User found", 20, 0);
            return;
        }
    }
    send(socket, "User not found!", 15, 0);
}

/*Checking is password is correct*/
void checkpw(char *input, int socket, int user) {
    if(!strcmp((pws+userList[user].user), input)) {
        userList[user].pass = 1;
        send(socket, "Logged in!", 15, 0);
        return;
    }
    send(socket, "Wrong password", 20, 0);
}

/*void put(char *filename) {

}*/

//Send current working dir to client
void pwd(int socket, int user) {
    char *cwd = malloc(BUFSIZE*sizeof(char));
    strcpy(cwd, userList[user].currDir);
    send(socket, cwd, BUFSIZE, 0);
    free(cwd);
}

void ls(int socket, int user) {
    DIR *curdir;
    char *buff = malloc(BUFSIZE*sizeof(char));
    struct dirent *curfile;
    curdir = opendir(userList[user].currDir);
    char *nl="\n";
    while ((curfile = readdir(curdir)) != NULL) {
        strcat(buff, curfile->d_name);
        strcat(buff, nl);
    }
    printf("%s\n",buff );
    send(socket, buff, BUFSIZE, 0);
    free(buff);
}


/*Function that distinguishes the commands 
and calls for functions to execute them*/
void execute(char *input, int socket, int user) {

    char **args = tokenizeString(input);
    while(1) {
        if(strcmp(args[0], "USER")==0) {           
            if(userList[user].user==-1) 
                findUser(args[1], socket, user);
            else
                send(socket, "User already selectied!", 25, 0);
            return;

        }
        if(strcmp(args[0], "PASS")==0) {
            if(userList[user].user==-1) {
                send(socket, "Select user!", 25, 0);
                return;
            }
            if(!userList[user].pass)
                checkpw(args[1], socket, user);
            else
                send(socket, "User already logged in!", 25, 0);
            return;
        }
        if(strcmp(args[0], "PUT")==0) {
            if(args[1] == NULL) {
                send(socket, "No file specified!", 25, 0);
                return;
            }
            //put(args[1]);
            return;
        }
        if(strcmp(args[0], "PWD")==0) {
            printf("123\n");
            pwd(socket, user);
            return;
        }
        if(strcmp(args[0], "LS")==0) {
            //printf("EDEN\n");
            ls(socket, user);
            return;
        }
        //break;
    }
    //printf("%s\n",  );
}