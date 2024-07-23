
#include <Windows.h>
#include <iostream>
#include <thread>

#include "JVMClient/CheatMain/CheatMain.h"

BOOL __stdcall DllMain(HINSTANCE hInstance ,DWORD reason,LPVOID Reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
#ifdef DEBUG
        AllocConsole();
        freopen_s(&Globals::ConsoleFile, "CONOUT$", "w", stdout);
#endif

        Globals::mainThread = std::thread(MainThread, hInstance);
        Globals::mainThread.detach();

        break;

    case DLL_PROCESS_DETACH:
#ifdef DEBUG
        fclose(Globals::ConsoleFile);
        FreeConsole();
#endif
        break;
    }

    return true;
}

