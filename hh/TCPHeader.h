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
#define SA struct sockaddr
#define delim " \t\r\n\a"
#define BUFSIZE 1024