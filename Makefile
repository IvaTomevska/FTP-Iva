+all: FTPServer FTPClient

FTPServer: FTPServer.o
	gcc FTPServer.o -o FTPServer

FTPServer.o: FTPServer.c ServerHeader.h TCPHeader.h
	gcc -c FTPServer.c

FTPClient: FTPClient.o
	gcc FTPClient.o -o FTPClient

FTPClient.o: FTPClient.c ClientHeader.h TCPHeader.h
	gcc -c FTPClient.c

clean:
	rm *.o FTPClient FTPServer
