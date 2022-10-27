#pragma once
#include "winapi.hpp"

#include <ddraw.h>

#include <string>
#include <vector>

#include <fmt/core.h>

#include "types.hpp"
#include "offsets.hpp"
#include "core.hpp"

class Console 
{
private:
	static FILE* fDummy;
public:
	static void initialize();
	static void deinitialize();
};

class App
{
public:
	static App* getInstance();
	static void deleteInstance();
private:
	static App* appInstance;
public:
	void run();
	void handleInput();
	void detach();
public:
	uintptr_t base;
	AppClass* appClass;
	std::vector<StaticCoin*> stars;
	t_Blt o_Blt;
	LPDIRECTDRAWSURFACE lpSurf;
};