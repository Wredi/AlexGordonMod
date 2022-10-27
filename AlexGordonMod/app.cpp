#include "app.h"

HRESULT __stdcall BltHook(void* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	App::getInstance()->handleInput();
	AppClass* appClass = App::getInstance()->appClass;
	Alex* alex = appClass->ptrGameObject->ptrToChad;

	if (lpSrcRect->right == 800 && lpSrcRect->bottom == 600) {
		if (appClass) {
			HDC hdc;
			if (lpDDSrcSurface->GetDC(&hdc) == DD_OK)
			{
				const auto toScreen = [&](const Vector2& in) {
					return Vector2{
						.x = 800 + (in.x - appClass->ptrGameObject->ptrEntityListWrapper->realCameraPos.x),
						.y = 600 + (in.y - appClass->ptrGameObject->ptrEntityListWrapper->realCameraPos.y)
					};
				};
				Vector2 alexOnScreen = toScreen(appClass->ptrGameObject->ptrToChad->pos);
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
	}
	return App::getInstance()->o_Blt(This, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

App* App::appInstance = nullptr;
FILE* Console::fDummy = nullptr;

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

void App::run()
{
	Console::initialize();

	this->base = (uintptr_t)(GetModuleHandle(NULL));
	this->appClass = *(AppClass**)(base + 0x22F224);

	if (this->appClass) {
		this->stars = getAllStars(appClass->ptrGameObject);

		this->appClass->ptrToRenderer->lpDirectDraw->GetGDISurface(&this->lpSurf);
		this->o_Blt = MakeVmtHook<t_Blt>(*(t_Blt**)this->lpSurf, 5, BltHook);
	}
	else {
		debug_log("App pointer is invalid");
	}
}

void App::handleInput()
{
	if (GetAsyncKeyState(VK_END) & 0x01) {
		stars = getAllStars(this->appClass->ptrGameObject);
	}

	if (GetAsyncKeyState(VK_HOME) & 0x1) {
		auto ents = getEntsByVMT<BasicEntity*>(this->appClass->ptrGameObject->ptrEntityListWrapper, ENTRANCE_VTABLE);
		auto alex = this->appClass->ptrGameObject->ptrToChad;
		alex->pos = ents[0]->pos;
	}
}

void App::detach()
{
	debug_log("DEALOCATION RESOURCES");

	MakeVmtHook<t_Blt>(*(t_Blt**)this->lpSurf, 5, o_Blt);
	Sleep(10);

	Console::deinitialize();
}

void Console::initialize()
{
	AllocConsole();
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

void Console::deinitialize()
{
	if (fDummy) fclose(fDummy);
	FreeConsole();
}