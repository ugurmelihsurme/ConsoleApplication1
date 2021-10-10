// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include "resource.h"
//#include "WindowProcedure.h"
#include "Console.h"

int main(void )
{
    Console* console = new WinConsole("Melih Console");
        
    console->readInputBufferEvent(console->getHandle(), console->getPrevMode());
    
      
    return 0;
}
