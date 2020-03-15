#include "TCPHeader.h"
char *read_line(void);
char** tokenizeString(char *input);
int execute(char **args);
void pwd(void);
void cd (char **args);
void ls(void);
void get(int socket, char *filename);
void put(int socket, char *filename);