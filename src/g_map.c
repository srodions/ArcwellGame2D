#include "g_map.h"
#include "i_system.h"
#include "l_arcloader.h"
#include "typedefs.h"
#include "fixed_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
 * This new function is not compatible with Android platform
 * TODO: Use SDL I/O methods in future
 */
map_t G_MapInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, const char* name)
{
	uint32_t currentFileSize = 0;
	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) L_LoadLump(arcFile, name, pHeader, pTable, &currentFileSize);

	uint32_t rows = mapDataHeader->mapRows;
	uint32_t columns = mapDataHeader->mapColumns;

	const int totalTiles = rows * columns;
	char* mapData = mapDataHeader->data;

	map_t location;
	location.rows = rows;
	location.columns = columns;
	location.tileAtlasIdx = mapDataHeader->tileAtlasIdx;
	location.locationTiles = (tile_t*) malloc(totalTiles * sizeof(tile_t));

	// WORKING WITH DATA
	int tempY = 0;
	for (uint32_t y = 0; y < rows; ++y)
	{
	    int tempX = 0;

	    for (uint32_t x = 0; x < columns; ++x)
	    {
	        char tile = mapData[y * columns + x];
	        int srcX = -1, srcY = -1;

	        if (tile >= 'A' && tile <= 'Z')
	        {
	            int index = tile - 'A';          					// Getting tile index (0 for 'A', 1 for 'B', ...)
	            srcX = (index % TLS_IN_ATLS_ROW) * TILE_SPR_SIZE; 	// Offset on X (the row has 5 tiles)
	            srcY = (index / TLS_IN_ATLS_ROW) * TILE_SPR_SIZE; 	// Offset on Y (new line break after 5th tile)
	        }

	        location.locationTiles[y * columns + x] = G_TileInit(srcX, srcY, tempX, tempY);
	        tempX += TILE_SPR_SIZE;
	    }

	    tempY += TILE_SPR_SIZE;
	}

	if (mapDataHeader != NULL) free(mapDataHeader);

	return location;
}

void G_MapSetter(map_manager_t* pMapManager, FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, const char* name)
{
	int mapsCount = pMapManager->mapsCount;

	pMapManager->maps[mapsCount] = G_MapInit(arcFile, pHeader, pTable, name);

	++pMapManager->mapsCount;
}

void G_ObjInit(obj_manager_t* pObjManager, int sprIndex, int bsx, int bsy, int btx, int bty, bool isAnim)
{
	int objCount = pObjManager->objCount;

	pObjManager->animTimer[objCount].reactionTime = ANIM_TIME;
	pObjManager->isAnimated[objCount] = isAnim;

	pObjManager->transforms[objCount].logX = INT_TO_FIXED(btx * TILE_SPR_SIZE);
	pObjManager->transforms[objCount].logY = INT_TO_FIXED(bty * TILE_SPR_SIZE);

	pObjManager->sprites[objCount].currentSprite = 0;
	pObjManager->sprites[objCount].srcX = bsx * TILE_SPR_SIZE;
	pObjManager->sprites[objCount].srcY = bsy * TILE_SPR_SIZE;
	pObjManager->sprIndex[objCount] = sprIndex;

	++pObjManager->objCount;
}

void G_ObjSetter(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager)
{
	uint32_t objEntrySize = 0;
	arcf_objheader_t* objHeader = (arcf_objheader_t*) L_LoadLump(arcFile, "OBJECTS", pHeader, pTable, &objEntrySize);

	int objCount = objHeader->objCount;

	pObjManager->transforms = (obj_tform_t*) malloc(objCount * sizeof(obj_tform_t));
	pObjManager->sprites = (obj_sprite_t*) malloc(objCount * sizeof(obj_sprite_t));
	pObjManager->sprIndex = (int*) malloc(objCount * sizeof(int));
	pObjManager->animTimer = (rtimer_t*) malloc(objCount * sizeof(rtimer_t));
	pObjManager->isAnimated = (bool*) malloc(objCount * sizeof(bool));

	for (int i = 0; i < objCount; ++i)
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
