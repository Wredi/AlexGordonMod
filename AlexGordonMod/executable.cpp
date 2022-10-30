#include "executable.h"

Executable::Executable() : base((uintptr_t)GetModuleHandle(NULL)) {}

void Executable::nop(uintptr_t off, size_t bytesNum)
{
	void* addr = this->addrFromOff<void*>(off);
	DWORD old;
	VirtualProtect(addr, bytesNum, PAGE_EXECUTE_READWRITE, &old);
	memset(addr, 0x90, bytesNum);
	VirtualProtect(addr, bytesNum, old, &old);
}

void Executable::writeByte(uintptr_t off, u8 val)
{
	u8* addr = this->addrFromOff<u8*>(off);
	*addr = val;
}
