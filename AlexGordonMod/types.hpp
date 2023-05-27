#pragma once
#include <ddraw.h>

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

typedef long long i64;
typedef int i32;
typedef short i16;
typedef char i8;

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef float f32;
typedef double f64;

using t_Blt = HRESULT(__stdcall*)(void*, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
using t_Stupid_Malloc = void*(__cdecl*)(int Size);

class AppClass
{
public:
	char pad_0000[876]; //0x0000
	class Renderer* ptrToRenderer; //0x036C
	char pad_0370[776]; //0x0370
	class GeneralAppProperties* ptrGameObject; //0x0678
}; //Size: 0x067C
static_assert(sizeof(AppClass) == 0x67C);

class Renderer
{
public:
	char pad_0000[88]; //0x0000
	LPDIRECTDRAW lpDirectDraw; //0x0058
	uintptr_t* externalDirectDraw; //0x005C
	char pad_0060[228]; //0x0060
}; //Size: 0x0144
static_assert(sizeof(Renderer) == 0x144);


class GeneralAppProperties
{
public:
	char pad_0000[168]; //0x0000
	class EntListWrapper* ptrEntityListWrapper; //0x00A8
	char pad_00AC[4]; //0x00AC
	Vector2 retardCameraPos; //0x00B0
	Vector2 cameraVel;
	char pad_00BC[60]; //0x00BC
	class Alex* ptrToChad; //0x00FC
	char pad_0100[4]; //0x0100
	i32 moneyOnLevel; //0x0104
	char pad_0108[4]; //0x0108
	float health; //0x010C
	char pad_0110[20]; //0x0110
	i32 coinCounter; //0x0124
	char pad_0128[4960]; //0x0128
	i32 starsOnRun; //0x1488
	i32 totalStarsOnLevel; //0x148C
	i32 totalStarsFound; //0x1490
}; //Size: 0x1494
static_assert(sizeof(GeneralAppProperties) == 0x1494);

class EntListWrapper
{
public:
	char pad_0000[164]; //0x0000
	uintptr_t** allEntities; //0x00A4
	char pad_00A8[4]; //0x00A8
	i32 numEntities; //0x00AC
	char pad_00B0[4]; //0x00B0
	uintptr_t** localEntities; //0x00B4
	char pad_00B8[88]; //0x00B8
	Vector2 realCameraPos; //0x0110
}; //Size: 0x0118
static_assert(sizeof(EntListWrapper) == 0x118);

class BasicEntity
{
public:
	uintptr_t vTable;
	char pad_0000[8];
	Vector2 pos;
};

class StaticCoin
{
public:
	char pad_0000[12]; //0x0000
	Vector2 pos; //0x000C
	char pad_0014[24]; //0x0014
	class EntAttr* ptrToEntAttr; //0x002C
	i8 wasntCollectedInPast; //0x0030
	char pad_0031[59]; //0x0031
	i8 wasntCollectedInRun; //0x006C
	char pad_006D[459]; //0x006D
	i8* ptrId; //0x0238
}; //Size: 0x023C
static_assert(sizeof(StaticCoin) == 0x23C);


class EntAttr
{
public:
	char pad_0000[4]; //0x0000
	i16 groupId; //0x0004
}; //Size: 0x0006
static_assert(sizeof(EntAttr) == 0x6);

class Alex
{
public:
	char pad_0000[12]; //0x0000
	Vector2 pos; //0x000C
	char pad_0014[452]; //0x0014
	Vector2 checkPoint; //0x01D8
}; //Size: 0x01E0
static_assert(sizeof(Alex) == 0x1E0);
