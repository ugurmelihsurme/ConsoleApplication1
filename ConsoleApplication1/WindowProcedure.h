#pragma once
#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "TrayView.h"

UINT WM_TASKBAR = 0;
HMENU Hmenu;

/*procedures  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void minimize(HWND hwnd);
void restore(HWND hwnd);



void minimize(HWND hwnd)
{
	// hide the main window
	ShowWindow(hwnd, SW_HIDE);
}


void restore(HWND hwnd)
{
	ShowWindow(hwnd, SW_SHOW);
}

// Step 4: the Window Procedure
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (/*message == WM_TASKBAR &&*/ !IsWindowVisible(hwnd))
	{
		minimize(hwnd);
		return 0;
	}

	switch (message)                  /* handle the messages */
	{
	case WM_ACTIVATE:
		//Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		if (!AddTrayIcon(hwnd,11, ::LoadIcon(0, MAKEINTRESOURCE(IDI_ICON1)), LPTSTR("Test")))
		return -1;

		//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
		break;
	case WM_CREATE:

		ShowWindow(hwnd, SW_HIDE);
		Hmenu = CreatePopupMenu();
		AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit The Demo"));

		break;

	case WM_SYSCOMMAND:
		/*In WM_SYSCOMMAND messages, the four low-order bits of the wParam parameter
		are used internally by the system. To obtain the correct result when testing the value of wParam,
		an application must combine the value 0xFFF0 with the wParam value by using the bitwise AND operator.*/

		switch (wParam & 0xFFF0)
		{
		case SC_MINIMIZE:
		case SC_CLOSE:
			minimize(hwnd);
			return 0;
			break;
		}
		break;


		// Our user defined WM_SYSICON message.
	case WM_SYSICON:
	{

		switch (wParam)
		{
		case ID_TRAY_APP_ICON:
			SetForegroundWindow(hwnd);

			break;
		}


		if (lParam == WM_LBUTTONUP)
		{

			restore(hwnd);
		}
		else if (lParam == WM_RBUTTONDOWN)
		{
			// Get current mouse position.
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(hwnd);

			// TrackPopupMenu blocks the app until TrackPopupMenu returns

			UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);



			SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
			if (clicked == ID_TRAY_EXIT)
			{
				// quit the application.
				Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
				PostQuitMessage(0);
			}
		}
	}
	break;

	// intercept the hittest message..
	case WM_NCHITTEST:
	{
		UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;
	}

	case WM_CLOSE:

		minimize(hwnd);
		return 0;
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
