#include "app.h"

#include "file-reversing.hpp"
#include <fstream>

HRESULT __stdcall BltCallback(void* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	if (lpSrcRect->right == 800 && lpSrcRect->bottom == 600) {
		App::getInstance()->handleInput();
		AppClass* appClass = App::getInstance()->appClass;

		if (!appClass) {
			return App::getInstance()->o_Blt(This, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
		}

		Alex* alex = appClass->ptrGameObject->ptrToChad;
		fmt::print("X: {:.2f} Y: {:.2f}\n", alex->pos.x, alex->pos.y);

		HDC hdc;
		if (lpDDSrcSurface->GetDC(&hdc) == DD_OK)
		{
			const auto toScreen = [&](const Vector2& in) {
				return Vector2{
					.x = 800 + (in.x - appClass->ptrGameObject->ptrEntityListWrapper->realCameraPos.x),
					.y = 600 + (in.y - appClass->ptrGameObject->ptrEntityListWrapper->realCameraPos.y)
				};
			};
			Vector2 alexOnScreen = toScreen(alex->pos);
			alexOnScreen.y -= 50.0f;

			COLORREF qLineColor = RGB(255, 0, 0);
			HPEN hLinePen = CreatePen(PS_SOLID, 1, qLineColor);

			SelectObject(hdc, hLinePen);

			for (auto i : App::getInstance()->stars)
			{
				if (i->wasntCollectedInPast && i->wasntCollectedInRun) {
					Vector2 starOnScreen = toScreen(i->pos);
					MoveToEx(hdc, int(alexOnScreen.x), int(alexOnScreen.y), NULL);
					LineTo(hdc, int(starOnScreen.x), int(starOnScreen.y));
				}
			}

			DeleteObject(hLinePen);

			lpDDSrcSurface->ReleaseDC(hdc);
		}
	}
	return App::getInstance()->o_Blt(This, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

App* App::appInstance = nullptr;

App* App::getInstance()
{
	if (!App::appInstance)
		App::appInstance = new App();
	return App::appInstance;
}

void App::deleteInstance()
{
	if (App::appInstance) {
		delete App::appInstance;
		App::appInstance = nullptr;
	}
}

void App::attach()
{
	/*this->appClass = *exec.addrFromOff<AppClass**>(APP_CLASS_OFFSET);
	if (!this->appClass) return utils::debug_log("App pointer is invalid");

	this->stars = utils::getAllStars(appClass->ptrGameObject);

	this->exec.nop(0x40F1B, 28);
	this->exec.nop(0x40FA7, 5);
	this->exec.nop(0x1D33C, 10);
	this->exec.nop(0x1D2DC, 2);
	this->exec.nop(0x1D2E7, 6);
	this->exec.writeByte(0x22F218, 1);

	LPDIRECTDRAWSURFACE lpSurf;
	this->appClass->ptrToRenderer->lpDirectDraw->GetGDISurface(&lpSurf);
	this->o_Blt = utils::MakeVmtHook<t_Blt>(*(t_Blt**)lpSurf, 5, BltCallback);*/

	//this->o_Stupid_Malloc = (t_Stupid_Malloc)utils::tramp_hook((char*)STUPID_MALLOC_ADDR, (char*)StupidMallocCallback, 5);

	uintptr_t addr = 0x0F27F020;
	setupMemoryHandler((void*)addr, FILE_2_SIZE);
}

void App::handleInput()
{
	if (GetAsyncKeyState(VK_END) & 0x1) {
		stars = utils::getAllStars(this->appClass->ptrGameObject);
	}

	if (GetAsyncKeyState(VK_HOME) & 0x1) {
		auto ents = utils::getEntsByVMT<BasicEntity*>(this->appClass->ptrGameObject->ptrEntityListWrapper, ENTRANCE_VTABLE);
		auto alex = this->appClass->ptrGameObject->ptrToChad;
		alex->pos = ents[0]->pos;
	}
}

void App::detach()
{
	utils::debug_log("DEALOCATION");

	std::ofstream file{ "lol.txt" };
	for (int i = 1; i < addresses.size(); ++i) {
		file << fmt::format("Addr lol: {}", addresses[i] - addresses[i - 1]) << '\n';
	}
	file.flush();

	/*LPDIRECTDRAWSURFACE lpSurf;
	this->appClass->ptrToRenderer->lpDirectDraw->GetGDISurface(&lpSurf);
	utils::MakeVmtHook<t_Blt>(*(t_Blt**)lpSurf, 5, o_Blt);*/

	//DWORD old_protection;
	//VirtualProtect((void*)STUPID_MALLOC_ADDR, 5, PAGE_EXECUTE_READWRITE, &old_protection);
	//memcpy_s((void*)STUPID_MALLOC_ADDR, 5, this->o_Stupid_Malloc, 5);
	//VirtualProtect(this->o_Stupid_Malloc, 5, old_protection, &old_protection);

	RemoveVectoredExceptionHandler(this->handler);

	Sleep(10);
}

Console::Console()
{
	AllocConsole();
	freopen_s(&this->fDummy, "CONOUT$", "w", stdout);
}

Console::~Console()
{
	if (this->fDummy) fclose(this->fDummy);
	FreeConsole();
}
