#pragma once
#include <windows.h>
#include <tchar.h>
#include "resource.h"
::NOTIFYICONDATA notifyIconData;

bool AddTrayIcon(HWND hwnd,UINT nID, ::HICON hIcon, LPTSTR lpszTip)
{
    if (!hIcon || !lpszTip)
        return false;

  
    notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData.hWnd = hwnd;
    notifyIconData.uID = nID;
    notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    notifyIconData.uCallbackMessage = 0;
    notifyIconData.hIcon = hIcon;
    lstrcpy(notifyIconData.szTip, lpszTip);

    return (::Shell_NotifyIcon(NIM_ADD, &notifyIconData) ? true : false);
}

//if (!AddTrayIcon(11, ::LoadIcon(0, MAKEINTRESOURCE(IDI_ICON1)), _T("Test")))
//return -1;
//
//::ShowWindow(::GetConsoleWindow(), SW_HIDE);