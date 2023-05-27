#include "utils.hpp"

void utils::debug_log(const char* str)
{
	OutputDebugStringA(str);
}

std::vector<StaticCoin*> utils::getAllStars(GeneralAppProperties* ptrGameObject)
{
	std::vector<StaticCoin*> out;

	if (!ptrGameObject) return out;

	EntListWrapper* ents = ptrGameObject->ptrEntityListWrapper;
	if (!ents || !ents->numEntities || !ents->allEntities) return out;

	for (int i = 0; i < ents->numEntities; ++i) {
		uintptr_t* obj = ents->allEntities[i];
		if (obj && *obj == STATIC_COIN_VTABLE) {
			StaticCoin* star = (StaticCoin*)obj;
			if (star->ptrToEntAttr && star->ptrToEntAttr->groupId == 1103) {
				if (star->ptrId && *star->ptrId == 18) {
					out.push_back(star);
				}
			}
		}
	}

	return out;
}

bool utils::detour(char* src, char* dst, const uintptr_t len)
{
    if (len < 5)
        return false;

    DWORD old_protection;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &old_protection);

    memset(src, 0x90, len);

    uintptr_t rel_addr = dst - src - 5;
    *src = 0xE9u;
    *(uintptr_t*)(src + 1) = rel_addr;

    VirtualProtect(src, len, old_protection, &old_protection);

    return true;
}

char* utils::tramp_hook(char* src, char* dst, const uintptr_t len)
{
	char* gateway = (char*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy_s(gateway, len, src, len);

	uintptr_t src_rel_addr = src - gateway - 5;
	*(gateway + len) = 0xE9u;
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = src_rel_addr;

	detour(src, dst, len);

	return gateway;
}