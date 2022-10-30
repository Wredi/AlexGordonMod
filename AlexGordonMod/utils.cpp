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