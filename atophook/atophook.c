
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SC_ALWAYS_ON_TOP (0xA010)

static BOOL bMenuItemRegistered;
static HINSTANCE hInst;

LRESULT
AtopAlwaysOnTopProc (
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
	)
{
	HMENU hSystemMenu;
	MENUITEMINFO menuItemInfo;
	BOOL ret, checked;

	/* Get the system menu for this window */
	hSystemMenu = GetSystemMenu(hWnd, FALSE);
	if (hSystemMenu == NULL)
		return 0;

	/* Query the current state of the menu item */
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_STATE;
	ret = GetMenuItemInfo(hSystemMenu, SC_ALWAYS_ON_TOP, FALSE, &menuItemInfo);
	if (ret == FALSE)
		return 0;

	/* Handle the state transition */
	checked = (menuItemInfo.fState & MFS_CHECKED) == MFS_CHECKED;
	if (checked)
	{
		/* Unset the topmost attribute */
		ret = SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		/* Uncheck the menu item if it was successful */
		if (ret)
			menuItemInfo.fState &= ~MFS_CHECKED;
	}
	else
	{
		/* Set the topmost attribute */
		ret = SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		/* Check the menu item if it was successful */
		if (ret)
			menuItemInfo.fState |= MFS_CHECKED;
	}

	/* Write the new state of the menu item */
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_STATE;
	SetMenuItemInfo(hSystemMenu, SC_ALWAYS_ON_TOP, FALSE, &menuItemInfo);

	return 0;
}

VOID
AtopInstallMenuItem (
	HWND hWnd
	)
{
	HMENU hSystemMenu;
	BOOL ret;

	/* Return quickly if we're already registered */
	if (bMenuItemRegistered)
		return;

	/* Get the system menu for this window */
	hSystemMenu = GetSystemMenu(hWnd, FALSE);
	if (hSystemMenu == NULL)
		return;

	/* Add a divider */
	AppendMenu(hSystemMenu, MF_SEPARATOR, SC_ALWAYS_ON_TOP + 1, NULL);

	/* Add an entry for ourselves */
	ret = AppendMenu(hSystemMenu, 0, SC_ALWAYS_ON_TOP, L"Always On Top");
	if (ret)
	{
		/* Set the flag if this worked */
		bMenuItemRegistered = TRUE;
	}
}


LRESULT
__declspec(dllexport)
CALLBACK
AtopWindowHook (
	INT nCode,
	WPARAM wParam,
	LPARAM lParam
	)
{
	/* We must not process if code < 0 */
	if (nCode < 0)
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	switch (nCode)
	{
	case HCBT_SYSCOMMAND:
		switch (LOWORD(wParam) & 0xFFF0)
		{
		case SC_ALWAYS_ON_TOP:
			/* Call our handler if this system menu message is ours */
			AtopAlwaysOnTopProc(GetForegroundWindow(), wParam, lParam);
			break;

		default:
			break;
		}
		break;

	case HCBT_SETFOCUS:
	case HCBT_ACTIVATE:
	case HCBT_CREATEWND:
		AtopInstallMenuItem((HWND)wParam);
		break;

	default:
		break;
	}

	/* Pass this message on */
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL
APIENTRY
DllMain (
	HINSTANCE hInstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hInst;
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

