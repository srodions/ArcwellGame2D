#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "l_location.h"
#include "s_system.h"
#include "l_arcloader.h"
#include "typedefs.h"

/*
 * This new function is not compatible with Android platform
 * TODO: Use SDL I/O methods in future
 */
location_t* L_LocationInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager)
{
	// LOADING DATA
	uint32_t currentFileSize = 0;
	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) L_LoadLump(arcFile, "TOMB", pHeader, pTable, &currentFileSize);
	uint32_t rows = mapDataHeader->mapRows;
	uint32_t columns = mapDataHeader->mapColumns;

	assert(rows <= MAX_MAP_ROWS && columns <= MAX_MAP_COLUMNS);

	const int totalTiles = rows * columns;
	char* mapData = (char*)(mapDataHeader + 1);

	location_t* location = (location_t*) malloc(sizeof(location_t));
	location->rows = rows;
	location->columns = columns;
	location->locationTiles = (tile_t*) malloc(totalTiles * sizeof(tile_t));

	// WORKING WITH DATA
	int tempY = 0;
	for (uint32_t y = 0; y < rows; ++y)
	{
		int srcX = 0;
		int srcY = 0;
		int tempX = 0;

		for (uint32_t x = 0; x < columns; ++x)
		{
			char currentTile = mapData[y * columns + x];

			switch (currentTile)
			{
			case 'A': 	srcX = 0; 						srcY = 0; break;
			case 'B': 	srcX = TILE_SPRITE_SIZE; 		srcY = 0; break;
			case 'C': 	srcX = TILE_SPRITE_SIZE * 2; 	srcY = 0; break;
			case 'D': 	srcX = TILE_SPRITE_SIZE * 3; 	srcY = 0; break;
			case 'E': 	srcX = TILE_SPRITE_SIZE * 4; 	srcY = 0; break;
			case 'F': 	srcX = 0; 						srcY = TILE_SPRITE_SIZE; break;
			case 'G': 	srcX = TILE_SPRITE_SIZE; 		srcY = TILE_SPRITE_SIZE; break;
			case 'H': 	srcX = TILE_SPRITE_SIZE * 2; 	srcY = TILE_SPRITE_SIZE; break;
			case 'I': 	srcX = TILE_SPRITE_SIZE * 3; 	srcY = TILE_SPRITE_SIZE; break;
			case 'J': 	srcX = TILE_SPRITE_SIZE * 4; 	srcY = TILE_SPRITE_SIZE; break;
			case '.':
			default: 	srcX = 1024; 					srcY = 1024; break;
			}

			location->locationTiles[y * columns + x] = L_TileInit(srcX, srcY, tempX, tempY);
			tempX += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
		}

		tempY += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	}

	if (mapDataHeader != NULL) free(mapDataHeader);

	return location;
}

void L_ObjectInit(obj_manager_t* pObjManager, enum OBJ_ID id, int bsx, int bsy, int btx, int bty, bool isAnim)
{
	assert(pObjManager->objCount <= MAX_OBJECTS);

	pObjManager->animTimer[pObjManager->objCount].reactionTime = ANIM_TIME;
	pObjManager->isAnimated[pObjManager->objCount] = isAnim;

	pObjManager->transforms[pObjManager->objCount].logX = (float) btx * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	pObjManager->transforms[pObjManager->objCount].logY = (float) bty * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;

	pObjManager->sprites[pObjManager->objCount].currentSprite = 0;
	pObjManager->sprites[pObjManager->objCount].srcX = bsx * TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].srcY = bsy * TILE_SPRITE_SIZE;
	pObjManager->id[pObjManager->objCount] = id;

	++pObjManager->objCount;
}

tile_t L_TileInit(int srcX, int srcY, int posX, int posY)
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

/*
 * This method implements map loading from a data file or any other ASCII file. (deprecated)
 */
/*
location_t L_LocationInit(SDL_Renderer* pRenderer, obj_manager_t* pObjManager)
{
	char* fullData = (char*) SDL_LoadFile("res/location/location.dat", NULL);
	if (fullData == NULL) printf("Location load error!\n");

	char* headerData = (char*) SDL_LoadFile("res/location/location.json", NULL);
	cJSON *json = cJSON_Parse(headerData);
	if (json == NULL)
	{
		const char* errorMsg = cJSON_GetErrorPtr();
		if (errorMsg != NULL)
			fprintf(stderr, "JSON Parse Error: %s\n", errorMsg);
		SDL_free(headerData);
	}

	location_t location = {
		.tileMap = IMG_LoadTexture(pRenderer, "res/tile/tiles.png"),
		.rows = cJSON_GetObjectItem(json, "rows")->valueint, 		// 39
		.columns = cJSON_GetObjectItem(json, "columns")->valueint, 	// 12
		.locationDest.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE,
		.locationDest.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE
	};

	assert(location.rows <= MAX_MAP_ROWS && location.columns <= MAX_MAP_COLUMNS);

	int currentCol = 0;
	int tempY = 0;
	char* nextLine = NULL;
	char* rowBuffer = strtok_r(fullData, "\n\r", &nextLine);

	while (rowBuffer != NULL && currentCol < location.columns)
	{
		int currentRow = 0;
		int tempX = 0;
		int srcX = 0;
		int srcY = 0;
		char* nextChar = NULL;
		char* currentChar = strtok_r(rowBuffer, " ", &nextChar);

		while (currentChar != NULL && currentRow < location.rows)
		{
			switch (*currentChar)
			{
			case 'A': 	srcX = 0; 						srcY = 0; break;
			case 'B': 	srcX = TILE_SPRITE_SIZE; 		srcY = 0; break;
			case 'C': 	srcX = TILE_SPRITE_SIZE * 2; 	srcY = 0; break;
			case 'D': 	srcX = TILE_SPRITE_SIZE * 3; 	srcY = 0; break;
			case 'E': 	srcX = TILE_SPRITE_SIZE * 4; 	srcY = 0; break;
			case 'F': 	srcX = 0; 						srcY = TILE_SPRITE_SIZE; break;
			case 'G': 	srcX = TILE_SPRITE_SIZE; 		srcY = TILE_SPRITE_SIZE; break;
			case 'H': 	srcX = TILE_SPRITE_SIZE * 2; 	srcY = TILE_SPRITE_SIZE; break;
			case 'I': 	srcX = TILE_SPRITE_SIZE * 3; 	srcY = TILE_SPRITE_SIZE; break;
			case 'J': 	srcX = TILE_SPRITE_SIZE * 4; 	srcY = TILE_SPRITE_SIZE; break;
			case '.':
			default: 	srcX = 1024; 						srcY = 1024; break;
			}

			location.locationTiles[currentCol][currentRow] = L_TileInit(srcX, srcY, tempX, tempY);
			tempX += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
			++currentRow;
			currentChar = strtok_r(NULL, " ", &nextChar);
		}

		tempY += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
		++currentCol;
		rowBuffer = strtok_r(NULL, "\n\r", &nextLine);
	}

	if (fullData != NULL)
		SDL_free(fullData);
	if (headerData != NULL)
		SDL_free(headerData);

	return location;
}
*/


