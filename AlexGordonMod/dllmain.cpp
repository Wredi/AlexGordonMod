#include "winapi.hpp"

#pragma comment( lib, "ddraw" )
#pragma comment( lib, "dxguid" )

#include "app.h"

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	App::getInstance()->run();
    while (true) {
        if (GetAsyncKeyState(VK_DELETE) & 0x01) {
            break;
        }
        Sleep(1);
    }
    App::getInstance()->detach();
    App::deleteInstance();
    FreeLibraryAndExitThread((HMODULE)lpThreadParameter, 0);
    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        if (const auto handle = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr)) {
            CloseHandle(handle);
        }
    }
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		App::getInstance()->detach();
		App::deleteInstance();
	}
    return TRUE;
}

