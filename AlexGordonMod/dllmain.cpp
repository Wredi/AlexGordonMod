#include "winapi.hpp"

#pragma comment( lib, "ddraw" )
#pragma comment( lib, "dxguid" )

#include "app.h"

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	App::getInstance()->run((HMODULE)lpThreadParameter);
	ExitThread(0);
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

