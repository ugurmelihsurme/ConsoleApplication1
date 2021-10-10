#pragma once
#include <windows.h>
#include <stdio.h>
#include "TrayView.h"


#define LDEBUG(f_, ...) printf((f_), __VA_ARGS__)
#define TM_SNPRINTF(s_, sz_, f_, ...) snprintf((s_), (sz_), (f_), __VA_ARGS__)

class Console
{
public:
	virtual void  errorExit(void * handle, const char* message, unsigned long prevMode)=0;
	virtual void  keyEventProc(KEY_EVENT_RECORD) =0;
	virtual void  mouseEventProc(MOUSE_EVENT_RECORD)=0;
	virtual void  resizeEventProc(WINDOW_BUFFER_SIZE_RECORD)=0;
	virtual HANDLE getStandardInputHandle(DWORD p_fdwSaveOldMode) = 0;
	virtual int readInputBufferEvent(HANDLE p_hStdin, DWORD p_fdwSaveOldMode) = 0;
	virtual DWORD setConsoleMode(HANDLE p_hStdin) = 0;
	virtual HANDLE getHandle() = 0;
	virtual DWORD getPrevMode() = 0;
};


class WinConsole : public Console
{
private:
	volatile bool isRun = true;
	HANDLE handle;
	DWORD prevMode = 0;
	HMENU popupMenu;
	HWND hwnd;
	HMENU hmenu;
public:
	WinConsole(const char* consoleTitle)
	{
		hwnd = GetConsoleWindow();
		hmenu = GetSystemMenu(hwnd, FALSE);
		EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
			
		std::wstring wc;
		wc.append(getWC(consoleTitle));
		SetConsoleTitle(wc.c_str());

		handle = getStandardInputHandle(prevMode);
		prevMode = setConsoleMode(handle);

	}
public:
	HANDLE getHandle() { return this->handle; }
	DWORD getPrevMode() { return this->prevMode; }
private:
	const wchar_t* getWC(const char* c)
	{
		size_t retval;
		const size_t cSize = strlen(c) + 1;
		std::wstring wc(cSize, L'#');
		mbstowcs_s(&retval,&wc[0],wc.size(), c, cSize);

		return (new std::wstring(wc))->c_str();
	}

private:
	DWORD setConsoleMode(HANDLE p_hStdin)
	{
		DWORD fdwMode;
		DWORD fdwSaveOldMode;
		// Save the current input mode, to be restored on exit.
		if (!GetConsoleMode(p_hStdin, &fdwSaveOldMode))
			errorExit(p_hStdin, LPSTR("GetConsoleMode"), fdwSaveOldMode);


		// Enable the window and mouse input events.
		fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(p_hStdin, fdwMode))
			errorExit(p_hStdin, LPSTR("SetConsoleMode"), fdwSaveOldMode);

		return fdwSaveOldMode;
	}

	HANDLE getStandardInputHandle(DWORD p_fdwSaveOldMode)
	{
		HANDLE r_hStdin;

		// Get the standard input handle.
		r_hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (r_hStdin == INVALID_HANDLE_VALUE)
		{
			errorExit(NULL, LPSTR("GetStdHandle"), p_fdwSaveOldMode);
		}

		return r_hStdin;
	}

public:
	int readInputBufferEvent(HANDLE p_hStdin, DWORD p_fdwSaveOldMode)
	{
		DWORD cNumRead, i;
		INPUT_RECORD irInBuf[128];
		
		// Loop to read and handle the next 100 input events.
		while (isRun)
		{
			// Wait for the events.
			if (!ReadConsoleInput(
				p_hStdin,      // input buffer handle
				irInBuf,     // buffer to read into
				128,         // size of read buffer
				&cNumRead)) // number of records read
				errorExit(p_hStdin, LPSTR("ReadConsoleInput"), p_fdwSaveOldMode);

			// Dispatch the events to the appropriate handler.

			for (i = 0; i < cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
				case KEY_EVENT: // keyboard input
					keyEventProc(irInBuf[i].Event.KeyEvent);
					break;

				case MOUSE_EVENT: // mouse input
					mouseEventProc(irInBuf[i].Event.MouseEvent);
					break;

				case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
					resizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
					break;

				case FOCUS_EVENT:  // disregard focus events
					break;
				case MENU_EVENT:   // disregard menu events
					//isRun = false;

					break;

				default:
					errorExit(p_hStdin, LPSTR("Unknown event type"), p_fdwSaveOldMode);
					break;
				}
			}
		}

		// Restore input mode on exit.

		SetConsoleMode(p_hStdin, p_fdwSaveOldMode);

		return 0;
	}
	private:
	void  errorExit(void* handle, const char* message, unsigned long prevMode)
	{
		fprintf(stderr, "%s\n", message);

		// Restore input mode on exit.

		SetConsoleMode(handle, prevMode);

		ExitProcess(0);
	}

	void  keyEventProc(KEY_EVENT_RECORD ker)
	{
		LDEBUG("Key event: ");
				
		if (ker.uChar.AsciiChar	== 'q')
		{
			this->isRun = false;
		}


	}

	void  mouseEventProc(MOUSE_EVENT_RECORD mer)
	{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
		LDEBUG("Mouse event: ");

		switch (mer.dwEventFlags)
		{
		case 0:

			if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				LDEBUG("left button press \n");
			}
			else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
			{
				LDEBUG("right button press \n");
					
			}
			else
			{
				LDEBUG("button press\n");
			}
			break;
		case DOUBLE_CLICK:
			LDEBUG("double click\n");
			break;
		case MOUSE_HWHEELED:
			LDEBUG("horizontal mouse wheel\n");
			break;
		case MOUSE_MOVED:
			LDEBUG("mouse moved\n");
			break;
		case MOUSE_WHEELED:
			LDEBUG("vertical mouse wheel\n");
			break;
		default:
			LDEBUG("unknown\n");
			break;
		}
	}

	void  resizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
	{
		LDEBUG("Resize event\n");
		LDEBUG("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
	}
};

