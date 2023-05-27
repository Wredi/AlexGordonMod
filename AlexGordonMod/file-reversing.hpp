#pragma once

#include <Windows.h>
#include "app.h"
#include <iostream>

constexpr int FILE_2_SIZE = 743567;
constexpr int CALL_FIRST = 1;
constexpr int CALL_LAST = 0;

MEMORY_BASIC_INFORMATION getMemoryInfo(void* objAddr, int objSize) {
	MEMORY_BASIC_INFORMATION memInfo;
	VirtualQuery(objAddr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

	return memInfo;
}

void setMemoryBreakpoint(void* objAddr, int objSize, bool reset) {
	MEMORY_BASIC_INFORMATION memInfo = getMemoryInfo(objAddr, objSize);

	DWORD cock;
	if (reset) {
		VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, PAGE_READWRITE, &cock);
	}
	else {
		DWORD NewProtect = memInfo.Protect | PAGE_GUARD;

	/*	std::cout << "memory protect initial?: 0x" << std::hex << memInfo.Protect << std::endl;
		std::cout << "hmmmm: 0x" << std::hex << memInfo.BaseAddress << std::endl;
		std::cout << "ehhh: " << std::dec << memInfo.RegionSize << std::endl;
		std::cout << "uwu: " << std::dec << 
			VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, NewProtect, &cock) << std::endl;*/
		VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, NewProtect, &cock);
	}
}

LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		ExceptionInfo->ContextRecord->EFlags |= 0x100;
		uintptr_t addr = (uintptr_t)(PVOID)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
		App* appInst = App::getInstance();
		if (addr >= appInst->objAddr && addr < appInst->objAddr + appInst->objSize) {

			if ((appInst->objAddr + appInst->objSize) - addr < 10) {
				setMemoryBreakpoint((void*)appInst->objAddr, appInst->objSize, true);
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			if (appInst->addresses.size() != 0) {
				if (addr > appInst->addresses.back()) {
					appInst->addresses.push_back(addr);
				}
			}
			else {
				appInst->addresses.push_back(addr);
			}

		}
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		ExceptionInfo->ContextRecord->EFlags &= ~0x100;
		setMemoryBreakpoint((PVOID)App::getInstance()->objAddr, App::getInstance()->objSize, false);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void setupMemoryHandler(void* addr, int size) {
	App::getInstance()->objAddr = (uintptr_t)addr;
	App::getInstance()->objSize = size;

	App::getInstance()->handler = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler);
	if (App::getInstance()->handler != NULL) {
		setMemoryBreakpoint(addr, size, false);
	}
}

void* __cdecl StupidMallocCallback(int size) {
	void* addr = App::getInstance()->o_Stupid_Malloc(size);

	if (size == FILE_2_SIZE) {
		setupMemoryHandler(addr, size);
	}

	return addr;
}
