#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <pwd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#define MAX_CLIENTS 30 
#define PORT 8093
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
    printf("Server listening and waiting for a connection..\n"); 
    
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
            //printf("Yes sex8\n");
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

//Send current working dir to client
void pwd(int socket, int user) {
    char *cwd = malloc(BUFSIZE*sizeof(char));
    strcpy(cwd, userList[user].currDir);
    send(socket, cwd, BUFSIZE, 0);
    free(cwd);
}

//List files in sirrent directory ftn
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

/*CD ftn, moves the directory of the server
chdir does the checks, tmp holds original locations,
after updating the client, go back to the original*/
void cd(char **args, int socket, int user) {
    char *path = malloc(BUFSIZE*sizeof(char));
    if(args[1]==NULL) {
        free(path);
        send(socket, "Invalid path\n", 15, 0);
        return;
    }
    strcpy(path, args[1]);
    char *temp = malloc(BUFSIZE*sizeof(char));
    char *buff = malloc(BUFSIZE*sizeof(char));
    getwd(buff);
    strcpy(temp,buff);
    chdir(userList[user].currDir);
    printf("%s\n", path);
    if(chdir(path) != 0) {
        send(socket, strerror(errno), BUFSIZE, 0);
        free(temp);
        free(path);
        return;
    }
    getwd(buff);
    strcpy(userList[user].currDir,buff);

    chdir(temp);
    free(temp);
    free(path);
    send(socket, buff, BUFSIZE, 0);

}

/*void put(char *filename, int socket, int user) {
    size_t instr;
    //printf("%s\n", filename);
    //int valread;
    //char *buff = malloc(BUFSIZE);
    //printf("ovde\n");
    if(read(socket, &instr, sizeof(size_t))<0) {
        perror("First read");
        //printf("%d\n",instr );
        return;
    }
    //valread=read(socket, &instr, sizeof(size_t));
    //printf("%d\n", valread);
    //printf("hehehehhe\n");
    if(!instr) {
        printf("Path or directory doesn't exist\n");
        return;
    }
    int file;
    //Create path
    char *path = malloc(BUFSIZE);
    strcpy(path, userList[user].currDir);
    char *slash = "/";
    strcat(path, slash);
    strcat(path, filename);
    path[strlen(path)] = '\0';
    //printf("%s\n", path);

    if((file = open(path, O_CREAT| O_WRONLY, 0666))<0) {
        perror("Opening");
        return;
    }
    printf("zzz\n");
    char *hold = malloc(instr);
    size_t r = 0;
    void *buf;
    size_t w;
    while(0<instr) {
        //printf("ppp\n");
        r+= read(socket, hold, instr);
        buf = hold;
        if(r<0) {
            perror("Second read");
            free(hold);
            close(file);
            free(path);
            return;
        }
        while(r>0) {
            //printf("nesho\n");
            w = write(file, buf, r);
            instr-=w;
            r-=w;
            buf+=w;
        }
    }
    printf("DONE\n");
    //send(socket, "File created", 25, 0);
    free(path);
    free(hold);
    close(file);
    return;
}*/

void get(char  **args, int socket, int user) {
    struct stat buf;
    //Create path
    char *path = malloc(BUFSIZE);
    strcpy(path, userList[user].currDir);
    char *slash = "/";
    strcat(path, slash);
    strcat(path, args[1]);
    path[strlen(path)] = '\0';

    stat(path, &buf);
    size_t size = buf.st_size;
    int file;
    if((file = open(path, O_RDONLY))<0) {
        perror("Opening error: ");
        size = 0;
        send(socket, &size, sizeof(size_t), 0);
        free (path);
        return;
    }
    send(socket, &size, sizeof(size_t), 0);
    sendfile(socket, file, NULL, size);
    printf("DONE\n");
    free(path);
    close(file);
    return;

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
            //put(args[1], socket, user);
            return 1;
        }

        if(strcmp(args[0], "PWD")==0) {
            //printf("123\n");
            pwd(socket, user);
            return;
        }

        if(strcmp(args[0], "LS")==0) {
            ls(socket, user);
            return;
        }

        if(strcmp(args[0], "CD")==0) {
            cd(args, socket, user);
            return;
        }

        if(strcmp(args[0], "GET")==0) {
            if(args[1] == NULL) {
                send(socket, "No file specified!", 25, 0);
                return;
            }
            get(args, socket, user);
            return 1;
        }
        //break;
    }
    //printf("%s\n",  );
}
