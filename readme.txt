Student: Iva Tomevska
NetID: it658
15.03.2020
Computer Networks Project 1: File Transfer Protocol
Description of code design

The project starts with $make once your in the directory. The Makefile creates two executables: TCPServer and TCPClient. Let's start with the Server.
Server:
Whent you execute TCPServer, the users need to register: to insert username and password for their accounts, which are storred in a list that is of type struct which storred the user's username, password and currend directory. Then the server creates a socket and once it is ready for connections it will display a message. Once a client is connected, it is ready to execute commands. It accepts connections from multile clients with the use of the select function and it stores the clients into a client array. Once they are storred into the array, it checks for messages from slients and once it recieves a message it calls the function execute which does the rest of the work.
All of the commands are written by the user in the client shell and the clients sends the commands to the server, and after execution the server returns a message to the client. When the server recieves a command and calles execute(), it splits(tokenizes with the finction tokenizeString())it into a string concicted of the command and the input(if needed for CD, USER, PASS). Then execute() calls appropriate functions based on the commands.
-Commands in server:
The command USER calls a function findUser() that checks in the user list if the user that is trying to log in is in registered(if their name is in the list), if it is there, but not logged in marks that it is loging in and asks for password. The command PASS calls a function checkpw() that check if the user is already logged in and if not it checks if the password is correct(from the user list and attribute .pass).
Command PWD calls a function pwd() that just gets the current working directory from the struct user attribute .currDir and sends it to the client.
Command LS calls a function ls() that also gets the current working directory from the struct user attribute .currDir, opens it, gets the names of the files (from the attribute d_name of the struct dirent object), storres them in an array and sends the array to the client.
Command CD calls a function cd() that also gets the current working directory from the struct user attribute .currDir, adds the name of the new directory to the path, checks if it's valid and changes it if it is.
Command PUT calls a function put() that opens a new connection to the client, recieves the file size first from the client, allocates memory in server directory and then recieves the data that was in the file from the client and writes into the file.
Command GET calls a function get() that opens a new connection to the client, sends the file size first and then the data of the file.
Client:
When the server is ready for a connection, we execute $FTPClient server-IP server-PORT. If IP and Port are not specified it won't execute. Then it creates a socket, connect to the server and asks for user input until user writes QUIT, a command wich will disconnect the clent from the server and terminate it.
Every command that doesn't start with '!' (except QUIT) the client sends to the server. The command which start with '!' the client executes itself with callinf the client function execute().
-Commands in client:
Command PWD calls a function pwd() that just gets the current working directory of the client with getwd() and prints it.
Command LS calls a function ls() that also gets the current working directory of the client with getwd(), opens it, gets the names of the files (from the attribute d_name of the struct dirent object), storres them in an array and sends the array to the client.
Command CD calls a function cd() that also gets the current working directory of the client with getwd(), adds the name of the new directory to the path, checks if it's valid and changes it if it is.
Command GET calls a function get() that opens a new connection to the server, recieves the file size first from the server, allocates memory in client directory and then recieves the data that was in the file from the server and writes into the file (communcater with get function in server).
Command PUT calls a function put() that opens a new connection to the server, sends the file size first and then the data of the file (communicates with put function in server).