#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct _ARCF_Header
{
	char signature[4]; 			// ARCF
	uint32_t lumpsCount;		// Files count in arc file
	uint32_t offsetToLumpsTable;// Offset to table of files in bytes
} arcf_header_t;

typedef struct _ARCF_MapEntryHeader
{
	uint32_t mapRows;
	uint32_t mapColumns;
} arcf_mapheader_t;

typedef struct _ARCF_ObjEntry
{
	uint32_t spriteIndex;
	uint32_t bySpriteX;
	uint32_t bySpriteY;
	uint32_t byTileX;
	uint32_t byTileY;
	bool isAnimated;
} arcf_objentry_t;

typedef struct _ARCF_ObjHeader
{
	uint32_t objCount;
	arcf_objentry_t items[];
} arcf_objheader_t;

typedef struct _ARCF_Entry
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

#endif /* TYPEDEFS_H_ */
