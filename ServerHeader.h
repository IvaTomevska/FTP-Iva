#include "TCPHeader.h"
int createAcc(void);
char** tokenizeString(char *input);
void findUser(char *input, int socket, int user);
void checkpw(char *input, int socket, int user);
void pwd(int socket, int user);
void ls(int socket, int user);
void cd(char **args, int socket, int user);
void get(char  **args, int socket, int user);
void put(char *filename, int socket, int user);
void execute(char *input, int socket, int user);
struct user
{
    int user;
    int pass;
    char *currDir;//we need the directory for multiple finctions

};