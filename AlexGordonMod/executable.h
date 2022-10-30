#pragma once
#include "winapi.hpp"

class Executable
{
public:
	Executable();

	template <typename T>
	T addrFromOff(uintptr_t off) {
		return reinterpret_cast<T>(base + off);
	}
private:
	uintptr_t base;
};

