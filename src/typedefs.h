#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct ARCF_Header
{
	char signature[4]; 			// ARCF
	uint32_t lumpsCount;		// Files count in arc file
	uint32_t offsetToLumpsTable;// Offset to table of files in bytes
} arcf_header_t;

typedef struct ARCF_MapEntryHeader
{
	uint32_t mapRows;
	uint32_t mapColumns;
	uint32_t tileAtlasIdx;
	char	data[];
} arcf_mapheader_t;

typedef struct ARCF_ObjEntry
{
	uint32_t spriteIndex;
	uint32_t bySpriteX;
	uint32_t bySpriteY;
	uint32_t byTileX;
	uint32_t byTileY;
	bool isAnimated;
} arcf_objentry_t;

typedef struct ARCF_ObjHeader
{
	uint32_t objCount;
	arcf_objentry_t items[];
} arcf_objheader_t;

typedef struct ARCF_Entry
{
    uint32_t offsetToFile;		// Offset to current file in the table in bytes
    uint32_t lumpSize;			// Size of current file in bytes
    char lumpName[16];			// The name(id) of the file to find it in the table
} arcf_entry_t;

typedef struct ARCF_SpriteHeader
{
	uint32_t spriteW;
	uint32_t spriteH;
} arcf_spriteheader_t;

typedef struct ARCF_NamesEntry
{
	uint32_t objCount;		// Amount of objects in file
	uint32_t entCount;		// ...
	uint32_t mapCount;		// ....
	uint32_t uiCount;		// .....
	char	objNames[256];	// Lines with names
	char	entNames[256];	// ...
	char	mapNames[256];	// ....
	char	uiNames[256];	// .....
} arcf_namesentry_t;

typedef struct EntityConfig
{
	uint32_t posX;
	uint32_t posY;
	uint32_t atlasSprIdx;
	// FRAME COUNTS
	uint32_t spawnFramesCount;
	uint32_t deathFramesCount;
	uint32_t walkFramesCount;
	uint32_t angerFramesCount;
	uint32_t attackFramesCount;
	uint32_t hurtFramesCount;
	// COMBAT
	uint32_t knockback;
	uint32_t speed;
	uint32_t strength;
	uint32_t maxHp;
	uint32_t attackDist;
	uint32_t dmgSpriteIndex; // Index of the sprite on which damage will be taken by the victim
	// ROWS
	uint32_t spawnFramesRow;
	uint32_t deathFramesRow;
	uint32_t angerFramesRow;
	uint32_t attackFramesRow;
	uint32_t hurtFramesRow;
	uint32_t walkFramesRow;
} e_config_t;

typedef struct ARCF_EntCfgHeader
{
	uint32_t cfgCount;
	e_config_t items[];
} arcf_entcfgheader_t;

#endif /* TYPEDEFS_H_ */
