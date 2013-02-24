// test.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

int installHook(void)
{
	HINSTANCE hModule;
	HOOKPROC hookProc;
	HHOOK hHook;

#ifdef _WIN64
	hModule = LoadLibrary(L"atophook64.dll");
#else
	hModule = LoadLibrary(L"atophook.dll");
#endif
	if (hModule == NULL)
	{
		wprintf(L"Failed to load the hook DLL: Error %d\n", GetLastError());
		return -1;
	}

	hookProc = (HOOKPROC)GetProcAddress(hModule, "AtopWindowHook");
	if (hookProc == NULL)
	{
		wprintf(L"Failed to find hook in DLL: Error %d\n", GetLastError());
		return -1;
	}

	hHook = SetWindowsHookEx(WH_CBT, hookProc, hModule, 0);
	if (hHook == NULL)
	{
		wprintf(L"Failed to install hook: Error %d\n", GetLastError());
		return -1;
	}

	return 0;
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	installHook();

	SuspendThread(GetCurrentThread());
	return 0;
}
