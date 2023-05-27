#pragma once
#include "winapi.hpp"

#include <ddraw.h>

#include <string>
#include <vector>

#include <fmt/core.h>

#include "types.hpp"
#include "offsets.hpp"
#include "utils.hpp"
#include "executable.h"

class Console 
{
private:
	FILE* fDummy;
public:
	Console();
	~Console();
};

class App
{
public:
	static App* getInstance();
	static void deleteInstance();
private:
	static App* appInstance;
public:
	void attach();
	void handleInput();
	void detach();
public:
	Executable exec;
	Console console;
	t_Blt o_Blt;

	t_Stupid_Malloc o_Stupid_Malloc;
public:
	std::vector<uintptr_t> addresses;
	uintptr_t objAddr;
	uintptr_t objSize;
	PVOID handler;
public:
	AppClass* appClass;
	std::vector<StaticCoin*> stars;
};