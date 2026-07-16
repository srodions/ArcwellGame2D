#include "g_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "i_system.h"
#include "l_arcloader.h"
#include "typedefs.h"
#include "fixed_math.h"

/*
 * This new function is not compatible with Android platform
 * TODO: Use SDL I/O methods in future
 */
map_t* G_MapInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager)
{
	// LOADING DATA
	uint32_t currentFileSize = 0;
	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) L_LoadLump(arcFile, "TOMB", pHeader, pTable, &currentFileSize);
	uint32_t rows = mapDataHeader->mapRows;
	uint32_t columns = mapDataHeader->mapColumns;

	assert(rows <= MAX_MAP_ROWS && columns <= MAX_MAP_COLUMNS);

	const int totalTiles = rows * columns;
	char* mapData = (char*)(mapDataHeader + 1);

	map_t* location = (map_t*) malloc(sizeof(map_t));
	location->rows = rows;
	location->columns = columns;
	location->locationTiles = (tile_t*) malloc(totalTiles * sizeof(tile_t));

	// WORKING WITH DATA
	int tempY = 0;
	for (uint32_t y = 0; y < rows; ++y)
	{
	    int tempX = 0;

	    for (uint32_t x = 0; x < columns; ++x)
	    {
	        char tile = mapData[y * columns + x];
	        int srcX = 1024, srcY = 1024;

	        if (tile >= 'A' && tile <= 'J')
	        {
	            int index = tile - 'A';          		// Getting tile index (0 for 'A', 1 for 'B', ...)
	            srcX = (index % 5) * TILE_SPRITE_SIZE; 	// Offset on X (the row has 5 tiles)
	            srcY = (index / 5) * TILE_SPRITE_SIZE; 	// Offset on Y (new line break after 5th tile)
	        }

	        location->locationTiles[y * columns + x] = G_TileInit(srcX, srcY, tempX, tempY);
	        tempX += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	    }

	    tempY += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	}

	if (mapDataHeader != NULL) free(mapDataHeader);

	return location;
}

void G_ObjInit(obj_manager_t* pObjManager, enum OBJ_ID id, int bsx, int bsy, int btx, int bty, bool isAnim)
{
	int objCount = pObjManager->objCount;
	assert(objCount <= MAX_OBJECTS);

	pObjManager->animTimer[objCount].reactionTime = ANIM_TIME;
	pObjManager->isAnimated[objCount] = isAnim;

	pObjManager->transforms[objCount].logX = INT_TO_FIXED(btx * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE);
	pObjManager->transforms[objCount].logY = INT_TO_FIXED(bty * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE);

	pObjManager->sprites[objCount].currentSprite = 0;
	pObjManager->sprites[objCount].srcX = bsx * TILE_SPRITE_SIZE;
	pObjManager->sprites[objCount].srcY = bsy * TILE_SPRITE_SIZE;
	pObjManager->id[objCount] = id;

	++pObjManager->objCount;
}

void G_ObjSetter(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager)
{
	uint32_t objEntrySize = 0;
	arcf_objheader_t* objHeader = (arcf_objheader_t*) L_LoadLump(arcFile, "OBJECTS", pHeader, pTable, &objEntrySize);

	for (int i = 0; i < objHeader->objCount; ++i)
	{
		int si = objHeader->items[i].spriteIndex;
		int bsx = objHeader->items[i].bySpriteX;
		int bsy = objHeader->items[i].bySpriteY;
		int btx = objHeader->items[i].byTileX;
		int bty = objHeader->items[i].byTileY;
		bool isAnim = objHeader->items[i].isAnimated;

		G_ObjInit(pObjManager, si, bsx, bsy, btx, bty, isAnim);
	}
}

tile_t G_TileInit(int srcX, int srcY, int posX, int posY)
{
	tile_t tile = {
		// Source tile
		.srcX = srcX,
		.srcY = srcY,
		// Destination tile
		.posX = posX,
		.posY = posY
	};

	return tile;
}
