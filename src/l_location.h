#ifndef L_LOCATION_H_
#define L_LOCATION_H_

#include "typedefs.h"

// --- DEFINES ---
location_t L_LocationInit(SDL_Renderer* pRenderer, char* locationName);
tile_t L_TileInit(int srcX, int srcY, int posX, int posY);

// --- IMPLEMENTATIONS ---
#if defined(STB_LOCATION_IMPLEMENTATION)
/*
 * This method implements map loading from a data file or any other ASCII file.
 */
location_t L_LocationInit(SDL_Renderer* pRenderer, char* locationName)
{
	location_t location = {
		// Location load
		.tileMap = IMG_LoadTexture(pRenderer, "res/tile/tiles.png"),
		.locationFile = fopen(locationName, "r"),
		.rows = 12,
		.columns = 32,
		.isNextLocation = false,
		// Location collision
		.leftWallLength = 10,
		.rightWallLength = 96
	};

	char rowBuffer[128];
	char* currentChar = NULL;
	int currentCol = 0;
	int tempY = 0;
	int srcY = 0;

	while (fgets(rowBuffer, sizeof(rowBuffer), location.locationFile) != NULL)
	{
		int currentRow = 0;
		int tempX = 0;
		int srcX = 0;

		currentChar = strtok(rowBuffer, " \n\r");
		while (currentChar != NULL)
		{
			switch (*currentChar)
			{
			case 'A':
				srcX = 0;
				srcY = 0;
				break;
			case 'B':
				srcX = TILE_SPRITE_SIZE;
				srcY = 0;
				break;
			case 'C':
				srcX = TILE_SPRITE_SIZE * 2;
				srcY = 0;
				break;
			case 'D':
				srcX = TILE_SPRITE_SIZE * 3;
				srcY = 0;
				break;
			case 'E':
				srcX = TILE_SPRITE_SIZE * 4;
				srcY = 0;
				break;
			case 'F':
				srcX = 0;
				srcY = TILE_SPRITE_SIZE;
				break;
			case 'G':
				srcX = TILE_SPRITE_SIZE;
				srcY = TILE_SPRITE_SIZE;
				break;
			case 'H':
				srcX = TILE_SPRITE_SIZE * 2;
				srcY = TILE_SPRITE_SIZE;
				break;
			case 'I':
				srcX = TILE_SPRITE_SIZE * 3;
				srcY = TILE_SPRITE_SIZE;
				break;
			case 'J':
				srcX = TILE_SPRITE_SIZE * 4;
				srcY = TILE_SPRITE_SIZE;
				break;
			}
			location.locationTiles[currentRow][currentCol] = L_TileInit(srcX, srcY, tempX, tempY);

			tempX += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
			++currentRow;
			currentChar = strtok(NULL, " \n\r");
		}

		tempY += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
		++currentCol;
	}

	fclose(location.locationFile);
	return location;
}


/*
 * This method implements tile initialization (sets tile texture getter position and its position on the screen).
 */
tile_t L_TileInit(int srcX, int srcY, int posX, int posY)
{
	tile_t tile = {
		// Source tile
		.tileSrc.x = srcX,
		.tileSrc.y = srcY,
		.tileSrc.w = TILE_SPRITE_SIZE,
		.tileSrc.h = TILE_SPRITE_SIZE,
		// Destination tile
		.posX = posX,
		.posY = posY
	};

	return tile;
}
#endif /* STB_LOCATION_IMPLEMENTATION */

#endif /* L_LOCATION_H_ */
