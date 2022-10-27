#pragma once

#include "winapi.hpp"
#include <vector>
#include <fmt/core.h>

#include "types.hpp"
#include "offsets.hpp"

template <typename... Args>
void debug_log(std::string_view fmt, Args&& ...args)
{
	std::string str = fmt::format(fmt, std::forward<Args>(args)...);
	OutputDebugStringA(str.c_str());
}

void debug_log(const char* str);

template <typename T>
T MakeVmtHook(T* vTable, int index, T hook)
{
	auto temp = vTable[index];
	vTable[index] = hook;
	return temp;
}

std::vector<StaticCoin*> getAllStars(GeneralAppProperties* ptrGameObject);

template <typename T>
std::vector<T> getEntsByVMT(EntListWrapper* entList, uintptr_t vTable)
{
	if (!entList || !entList->allEntities) return {};

	std::vector<T> out;
	for (int i = 0; i < entList->numEntities; ++i) {
		uintptr_t* ent = entList->allEntities[i];
		if (ent && *ent == vTable) out.push_back(std::bit_cast<T>(ent));
	}
	return out;
}