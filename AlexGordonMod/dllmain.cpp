#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <string>
#include <vector>

#pragma comment( lib, "ddraw" )
#pragma comment( lib, "dxguid" )

#include <ddraw.h>
#include <fmt/core.h>
#include <fstream>

#include "offsets.hpp"
#include <dump.hpp>
#include <iostream>

template <typename... Args>
void debug_log(std::string_view fmt, Args&& ...args)
{
	std::string str = fmt::format(fmt, std::forward<Args>(args)...);
	OutputDebugStringA(str.c_str());
}

void debug_log(const char* str)
{
	OutputDebugStringA(str);
}

std::vector<StaticCoin*> getAllStars(GeneralAppProperties* ptrGameObject) 
{
	std::vector<StaticCoin*> out;
	if (ptrGameObject) {
		EntListWrapper* ents = ptrGameObject->ptrEntityListWrapper;
		if (ents && ents->numEntities && ents->allEntities) {
			for (int i = 0; i < ents->numEntities; ++i) {
				uintptr_t* obj = ents->allEntities[i];
				if (obj && *obj == (uintptr_t)0x5FECFC) {
					StaticCoin* star = (StaticCoin*)obj;
					if (star->ptrToEntAttr && star->ptrToEntAttr->groupId == 1103) {
						if (star->ptrId && *star->ptrId == 18) {
							out.push_back(star);
						}
					}
				}
			}
		}
	}
	return out;
}

using t_Blt = HRESULT(__stdcall*)(void*, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
t_Blt o_Blt = nullptr;

AppClass* app;
std::vector<StaticCoin*> stars;
std::ofstream file;
HRESULT __stdcall BltHook(void* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	if (lpSrcRect->right == 800 && lpSrcRect->bottom == 600) {
		if (app->ptrGameObject) {
			HDC hdc;
			if (lpDDSrcSurface->GetDC(&hdc) == DD_OK)
			{
				const auto toScreen = [](const Vector2& in) {
					return Vector2{
						.x = 800 + (in.x - app->ptrGameObject->ptrEntityListWrapper->realCameraPos.x),
						.y = 600 + (in.y - app->ptrGameObject->ptrEntityListWrapper->realCameraPos.y)
					};
				};
				Vector2 alexOnScreen = toScreen(app->ptrGameObject->ptrToChad->pos);
				alexOnScreen.y -= 50.0f;

				COLORREF qLineColor = RGB(255, 0, 0);
				HPEN hLinePen = CreatePen(PS_SOLID, 1, qLineColor);

				SelectObject(hdc, hLinePen);

				for (auto i : stars)
				{
					i8 wasCollectedInPast = *(i8*)(uintptr_t(i) + 0x30);
					i8 wasCollectedInRun = *(i8*)(uintptr_t(i) + 0x6c);
					if (wasCollectedInPast != 0 && wasCollectedInRun != 0) {
						Vector2 starOnScreen = toScreen(i->pos);
						MoveToEx(hdc, int(alexOnScreen.x), int(alexOnScreen.y), NULL);
						LineTo(hdc, int(starOnScreen.x), int(starOnScreen.y));
					}
				}

				DeleteObject(hLinePen);

				lpDDSrcSurface->ReleaseDC(hdc);
			}
		}
	}
	return o_Blt(This, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

template <typename T>
T MakeVmtHook(T* vTable, int index, T hook)
{
	auto temp = vTable[index];
	vTable[index] = hook;
	return temp;
}

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	HMODULE hModule = (HMODULE)lpThreadParameter;

	file.open("log.txt");
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

    uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
    app = *(AppClass**)(base + 0x22F224);

	LPDIRECTDRAW dvc = app->ptrToRenderer->lpDirectDraw;
	LPDIRECTDRAWSURFACE lpSurf = nullptr;

	if (app) {
		stars = getAllStars(app->ptrGameObject);
		for (auto i : stars)
			fmt::print("{:X}\n", (uintptr_t)i);
		dvc->GetGDISurface(&lpSurf);
		o_Blt = MakeVmtHook(*(t_Blt**)lpSurf, 5, BltHook);
	}
	else {
		debug_log("App is pointing to garbage");
	}

    while (true)
    {
        if (GetAsyncKeyState(VK_DELETE) & 0x01) {
            break;
        }
		if (GetAsyncKeyState(VK_END) & 0x01) {
			stars = getAllStars(app->ptrGameObject);
		}
        Sleep(5);
    }
	Sleep(100);

	if (lpSurf != nullptr) {
		MakeVmtHook(*(t_Blt**)lpSurf, 5, o_Blt);
	}

	file.flush();
	file.close();
    debug_log("DEALOCATION RESOURCES SHIT FUCK");
	if(fDummy) fclose(fDummy);
	FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
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
            return TRUE;
        }
    }
    return FALSE;
}

