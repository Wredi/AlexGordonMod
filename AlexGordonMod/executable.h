#pragma once
#include "winapi.hpp"
#include "types.hpp"

class Executable
{
public:
	Executable();

	template <typename T>
	T addrFromOff(uintptr_t off) {
		return reinterpret_cast<T>(base + off);
	}

	void nop(uintptr_t off, size_t bytesNum);
	void writeByte(uintptr_t off, u8 val);
private:
	uintptr_t base;
};

