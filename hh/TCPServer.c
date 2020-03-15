#include "ServerHeader.h"
#define MAX_CLIENTS 30 
#define PORT 8094

char ** users;
char ** pws;
int usersNum = 0;


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

    printf("Server listening and waiting for a connection..\n"); 
    
    len = sizeof(cliaddr); 

    //Initializing attributes of user
    for (i=0; i<MAX_CLIENTS; i++) {
        userList[i].user = -1;
        userList[i].pass = 0;
        userList[i].currDir = malloc(BUFSIZE*sizeof(char));
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

        /*If something happened to master socket
        then it is an incoming connection*/
        if(FD_ISSET(sockfd_master, &read_fd_set)){

            if ((new_socket = accept(sockfd_master, (SA*)&cliaddr, &len))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            // Now server is ready to listen and verification 
            if ((listen(sockfd_master, 5)) != 0) { 
                printf("Listen failed...\n"); 
                exit(0); 
            } 

            //Add new socket to user array
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
                    execute(temp, sockfd, i);//this executes all the commands
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
    //check if it is smaller that BUFSIZE or empty
    if(input >= BUFSIZE) {
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

//Send current working dir of server to client
void pwd(int socket, int user) {
    char *cwd = malloc(BUFSIZE*sizeof(char));
    strcpy(cwd, userList[user].currDir);
    send(socket, cwd, BUFSIZE, 0);
    free(cwd);
}

/*Sends a list of files in current 
directory of server to client*/
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
        send(socket, "Invalid path", 15, 0);
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

/*Put recieves the file size first, allocates memory in server
directory and then recieves the data and writes into the file*/
void put(char *filename, int socket, int user) {
    size_t instr;
    if(read(socket, &instr, sizeof(size_t))<00) {
        perror("First read");
        return;
    }
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
        r+= read(socket, hold, instr);//reads from client file
        buf = hold;
        if(r<0) {
            perror("Second read");
            free(hold);
            close(file);
            free(path);
            return;
        }
        while(r>0) {
            w = write(file, buf, r);//writes into file
            instr-=w;
            r-=w;
            buf+=w;
        }
    }
    printf("DONE\n");
    free(path);
    free(hold);
    close(file);
    return;
}

/*Get sends the file size first and then the data
by opening a new connection*/
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
            put(args[1], socket, user);
            return 1;
        }

        if(strcmp(args[0], "PWD")==0) {
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
        send(socket, "Unknown command!", 20, 0);
        return;
    }
}
