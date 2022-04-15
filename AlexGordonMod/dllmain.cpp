#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <string>
#include <format>
#include <vector>
#include <sstream>
#include <fstream>

#pragma comment( lib, "ddraw" )
#pragma comment( lib, "dxguid" )

#include <ddraw.h>
#include <iostream>
#include "dump.hpp"

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

using int32_t = int;

class AppClass
{
public:
	char pad_0000[876]; //0x0000
	class Renderer* ptrToRenderer; //0x036C
	char pad_0370[776]; //0x0370
	class GeneralAppProperties* ptrGameObject; //0x0678
}; //Size: 0x067C
static_assert(sizeof(AppClass) == 0x67C);

class Renderer
{
public:
	char pad_0000[88]; //0x0000
	LPDIRECTDRAW lpDirectDraw; //0x0058
	uintptr_t* externalDirectDraw; //0x005C
	char pad_0060[228]; //0x0060
}; //Size: 0x0144
static_assert(sizeof(Renderer) == 0x144);


class GeneralAppProperties
{
public:
	char pad_0000[168]; //0x0000
	class EntListWrapper* ptrEntityListWrapper; //0x00A8
	char pad_00AC[4]; //0x00AC
	Vector3 retardCameraPos; //0x00B0
	char pad_00BC[64]; //0x00BC
	class Alex* ptrToChad; //0x00FC
	char pad_0100[4]; //0x0100
	int32_t moneyOnLevel; //0x0104
	char pad_0108[4]; //0x0108
	float health; //0x010C
	char pad_0110[20]; //0x0110
	int32_t coinCounter; //0x0124
	char pad_0128[4960]; //0x0128
	int32_t starsOnRun; //0x1488
	int32_t totalStarsOnLevel; //0x148C
	int32_t totalStarsFound; //0x1490
}; //Size: 0x1494
static_assert(sizeof(GeneralAppProperties) == 0x1494);

class EntListWrapper
{
public:
	char pad_0000[164]; //0x0000
	uintptr_t** allEntities; //0x00A4
	char pad_00A8[4]; //0x00A8
	int32_t numEntities; //0x00AC
	char pad_00B0[4]; //0x00B0
	uintptr_t** localEntities; //0x00B4
}; //Size: 0x00B8
static_assert(sizeof(EntListWrapper) == 0xB8);

class StaticCoin
{
public:
	char pad_0000[12]; //0x0000
	Vector2 pos; //0x000C
	char pad_0014[24]; //0x0014
	class EntAttr* ptrToEntAttr; //0x002C
	char pad_0030[520]; //0x0030
	int8_t* ptrId; //0x0238
}; //Size: 0x023C
static_assert(sizeof(StaticCoin) == 0x23C);

class EntAttr
{
public:
	char pad_0000[4]; //0x0000
	int16_t groupId; //0x0004
}; //Size: 0x0006
static_assert(sizeof(EntAttr) == 0x6);

class Alex
{
public:
	char pad_0000[12]; //0x0000
	Vector2 pos; //0x000C
}; //Size: 0x0014
static_assert(sizeof(Alex) == 0x14);

template <typename... Args>
void debug_log(std::string_view fmt, Args&& ...args)
{
	std::string str = std::format(fmt, std::forward<Args>(args)...);
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

using t_Flip = HRESULT(__stdcall*)(void*, LPDIRECTDRAWSURFACE, DWORD);
t_Flip o_Flip = nullptr;

using t_Blt = HRESULT(__stdcall*)(void*, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
t_Blt o_Blt = nullptr;

int index = 0;
std::ofstream file;
float lastTime = GetTickCount() * 0.001f;
//0X1000000
HRESULT __stdcall BltHook(void* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	if (lpSrcRect->right == 800 && lpSrcRect->bottom == 600) {
		float time = GetTickCount() * 0.001f;
		if (time - lastTime >= 1.0f) {
			lastTime = time;
			std::cout << "Fps: " << index << '\n';
			index = 0;
		}
		//std::stringstream ss;
		//ss << "Call number " << index++ << ":\n"
		//	<< std::format("\tDest Rect: {} {} | {} {}\n", lpDestRect->left, lpDestRect->top, lpDestRect->right, lpDestRect->bottom)
		//	<< std::format("\tSurface source pointer: {:#X}\n", (uintptr_t)lpDDSrcSurface)
		//	<< std::format("\tSource Rect: {} {} | {} {}\n", lpSrcRect->left, lpSrcRect->top, lpSrcRect->right, lpSrcRect->bottom)
		//	<< std::format("\tFlag combination: {:#X}\n", dwFlags)
		//	<< std::format("\tPointer to config struct: {:#X}\n\n", (uintptr_t)lpDDBltFx);
		//hex::dump bytes((byte*)lpDDBltFx, 100);
		//ss << bytes << "\n\n";
		//std::string out = ss.str();
		//std::cout << out;
		//file << out;
		++index;
	}

	return o_Blt(This, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

HRESULT __stdcall FlipHook(void* This, LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags) 
{
	std::cout << "Pozdro z flip\n";
	return o_Flip(This, lpDDSurfaceTargetOverride, dwFlags);
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
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	file.open("log.txt");
	if (!file.is_open()) debug_log("File shit");

    HMODULE hModule = (HMODULE)lpThreadParameter;

    uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
    AppClass* app = *(AppClass**)(base + 0x22F224);

	LPDIRECTDRAW dvc = app->ptrToRenderer->lpDirectDraw;
	LPDIRECTDRAWSURFACE lpSurf = nullptr;

	if (app) {
		const auto stars = getAllStars(app->ptrGameObject);
		for (auto i : stars)
		{
			debug_log("Position of star: {:.2f} {:.2f}", i->pos.x, i->pos.y);
		}
		debug_log("Device pointer: {:X}", (uintptr_t)dvc);

		dvc->GetGDISurface(&lpSurf);

		debug_log("Surface pointer: {:X}", (uintptr_t)lpSurf);

		o_Flip = MakeVmtHook(*(t_Flip**)lpSurf, 11, FlipHook);
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
        Sleep(5);
    }

	Sleep(100);

	if (lpSurf != nullptr) {
		MakeVmtHook(*(t_Flip**)lpSurf, 11, o_Flip);
		MakeVmtHook(*(t_Blt**)lpSurf, 5, o_Blt);
	}

    debug_log("DEALOCATION RESOURCES SHIT FUCK");

	file.flush();
	file.close();
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

