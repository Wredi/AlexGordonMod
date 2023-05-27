#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <string_view>
#include <format>

#include <assert.h>

using ulong_ptr = unsigned long;

/*std::cout << "-------------------------------------------------------\n";
std::cout << std::hex << "Address of object: 0x" << objAddr << '\n';
std::cout << std::hex << "Its size: 0x" << objSize << '\n';
std::cout << std::hex << "Address of closest starting memory page: 0x" << memInfo.BaseAddress << '\n';
std::cout << std::hex << "Address of base allocation addr: 0x" << memInfo.AllocationBase << '\n';
std::cout << std::hex << "Size of region: 0x" << memInfo.RegionSize << '\n';
std::cout << "-------------------------------------------------------";
std::cout << std::endl;*/

struct Enemy
{
	short type = 2;
	short resourceIndex = 10;
	int structSize = sizeof(Enemy);
	float x = 1.2f;
	float y = 233.54f;
	const char name[20] = "chair";
};

struct TestFile 
{
	const char magic[12] = "FILEv2.001";
	const char headerToCopy[80] = {};
	short levelType = 4;
	short pogType = 8000;
	int enemyArraySize = 3;
	Enemy enemies[3];
};

static_assert(sizeof(Enemy) == 36);
constexpr int desiredSize = 12 + 80 + 2 + 2 + 4 + (sizeof(Enemy) * 3);
static_assert(sizeof(TestFile) == desiredSize);

void accessFile(TestFile* lol) {
	std::cout << lol->magic << '\n';
	std::cout << lol->headerToCopy << '\n';
	std::cout << lol->levelType << '\n';
	std::cout << lol->pogType << '\n';
	std::cout << lol->enemyArraySize << '\n';

	for (int i = 0; i < lol->enemyArraySize; ++i) {
		std::cout << lol->enemies[i].type << '\n';
		std::cout << lol->enemies[i].resourceIndex << '\n';
		std::cout << lol->enemies[i].structSize << '\n';
		std::cout << lol->enemies[i].x << '\n';
		std::cout << lol->enemies[i].y << '\n';
		std::cout << lol->enemies[i].name << '\n';
	}
}

TestFile* objectAddr;
ulong_ptr objectSize = sizeof(TestFile);
DWORD old;

MEMORY_BASIC_INFORMATION getMemoryInfo(void* objAddr, int objSize) {
	MEMORY_BASIC_INFORMATION memInfo;
	VirtualQuery(objAddr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

	auto baseAddr = ulong_ptr(memInfo.BaseAddress);
	auto allocationBase = ulong_ptr(memInfo.AllocationBase);
	assert(baseAddr <= ulong_ptr(objectAddr));
	assert(allocationBase <= ulong_ptr(objectAddr));
	assert(memInfo.RegionSize >= objectSize);
	assert(baseAddr + memInfo.RegionSize >= ulong_ptr(objectAddr) + objectSize);

	return memInfo;
}
void setMemoryBreakpoint(void* objAddr, int objSize, bool reset) {
	MEMORY_BASIC_INFORMATION memInfo = getMemoryInfo(objAddr, objSize);

	if (reset) {
		DWORD cock;
		assert(VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, old, &cock));
	}
	else {
		DWORD NewProtect = memInfo.Protect | PAGE_GUARD;
		assert(VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, NewProtect, &old));
	}
}

ulong_ptr chuj[1000] = {0};
ulong_ptr lastAddr = 0;
ulong_ptr cipa = 0;
LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		ExceptionInfo->ContextRecord->EFlags |= 0x100;
		ulong_ptr addr = (ulong_ptr)(PVOID)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
		if (addr >= ulong_ptr(objectAddr) && addr < ulong_ptr(objectAddr) + objectSize) {
			if (addr > lastAddr) {
				chuj[cipa++] = addr;
				lastAddr = addr;
			}
		}
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		ExceptionInfo->ContextRecord->EFlags &= ~0x100;
		setMemoryBreakpoint((PVOID)objectAddr, objectSize, false);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

constexpr int CALL_FIRST = 1;
constexpr int CALL_LAST = 0;
int main() 
{
	PVOID h1 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler);
	assert(h1);

	objectAddr = new TestFile;
	std::cout << "0x" << std::hex << objectAddr << std::endl;

	setMemoryBreakpoint(objectAddr, sizeof(TestFile), false);
	accessFile(objectAddr);
	setMemoryBreakpoint(objectAddr, sizeof(TestFile), true);

	for (int i = 1; i < cipa; ++i) {
		std::cout << std::dec << "Cool address bro: " << chuj[i] - chuj[i-1] << '\n';
	}

	RemoveVectoredExceptionHandler(h1);

	delete objectAddr;

	return 0;
}


/*
bool setBreakpoint(void* addr, int size) {
	size_t pages = size / PAGE_SIZE;
	ulong_ptr memoryStart = (ulong_ptr)addr;

	if (memoryStart % PAGE_SIZE || !size || size % PAGE_SIZE) //ensure the data is aligned with the page size
		return false;

	MEMORY_BASIC_INFORMATION memInfo;
	DWORD oldProtect = 0;

	for (size_t i = 0; i < pages; i++)
	{
		const LPVOID curPage = (LPVOID)(memoryStart + i * PAGE_SIZE);

		VirtualQueryEx(GetCurrentProcess(), curPage, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (oldProtect == 0)
			oldProtect = memInfo.Protect;

		// Default to using PAGE_GUARD memory breakpoint
		if (!(memInfo.Protect & PAGE_GUARD))
		{
			DWORD newProtect = memInfo.Protect ^ PAGE_GUARD;
			VirtualProtectEx(GetCurrentProcess(), curPage, PAGE_SIZE, newProtect, &memInfo.Protect);
		}
	}
	return true;
}

*/