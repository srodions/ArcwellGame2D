#ifndef L_LOCATION_H_
#define L_LOCATION_H_

/* --- DEFINITIONS --- */
location_t L_LocationInit(SDL_Renderer* pRenderer, obj_manager_t* pObjManager, char* locationName);
tile_t L_TileInit(int srcX, int srcY, int posX, int posY);
void L_ObjectInit(obj_manager_t* pObjManager, int spriteIndex, int srcX, int srcY, int posX, int posY, bool isAnimated);
void L_ObjectSetter(obj_manager_t* pObjManager);
void L_Destruct(location_t* pLocation, obj_manager_t* pObjManager);

/* --- IMPLEMENTATIONS --- */
#if defined(STB_LOCATION_IMPLEMENTATION)

static SDL_Texture* obj_sprites[MAX_OBJ_SPRITES];

void L_ObjectSpritesInit(SDL_Renderer* pRenderer)
{
	obj_sprites[0] = IMG_LoadTexture(pRenderer, "res/object/torch.png");
	obj_sprites[1] = IMG_LoadTexture(pRenderer, "res/object/decoration.png");
	obj_sprites[2] = IMG_LoadTexture(pRenderer, "res/object/chest.png");
}

void L_ObjectSetter(obj_manager_t* pObjManager)
{
	int tileMove = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;
	// Torches
	L_ObjectInit(pObjManager, 0, 0, 0, tileMove * 3, 	tileMove * 4, true);
	L_ObjectInit(pObjManager, 0, 0, 0, tileMove * 11, 	tileMove * 4, true);
	L_ObjectInit(pObjManager, 0, 0, 0, tileMove * 19, 	tileMove * 4, true);
	L_ObjectInit(pObjManager, 0, 0, 0, tileMove * 27, 	tileMove * 4, true);
	L_ObjectInit(pObjManager, 0, 0, 0, tileMove * 35, 	tileMove * 4, true);
	// Chest
	L_ObjectInit(pObjManager, 2, 0, 0, tileMove * 11, 	tileMove * 6, false);
	// Column 1
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE, 		TILE_SPRITE_SIZE, tileMove * 7, tileMove * 6, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 7, tileMove * 5, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 7, tileMove * 4, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 3, 	TILE_SPRITE_SIZE, tileMove * 7, tileMove * 3, false);
	// Column 2
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE, 		TILE_SPRITE_SIZE, tileMove * 15, tileMove * 6, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 15, tileMove * 5, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 15, tileMove * 4, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 3, 	TILE_SPRITE_SIZE, tileMove * 15, tileMove * 3, false);
	// Column 3
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE, 		TILE_SPRITE_SIZE, tileMove * 23, tileMove * 6, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 23, tileMove * 5, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 23, tileMove * 4, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 3, 	TILE_SPRITE_SIZE, tileMove * 23, tileMove * 3, false);
	// Column 4
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE, 		TILE_SPRITE_SIZE, tileMove * 31, tileMove * 6, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 31, tileMove * 5, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 2, 	TILE_SPRITE_SIZE, tileMove * 31, tileMove * 4, false);
	L_ObjectInit(pObjManager, 1, TILE_SPRITE_SIZE * 3, 	TILE_SPRITE_SIZE, tileMove * 31, tileMove * 3, false);
}

/*
 * This method implements map loading from a data file or any other ASCII file.
 */
location_t L_LocationInit(SDL_Renderer* pRenderer, obj_manager_t* pObjManager, char* locationName)
{
	location_t location = {
		// Location load
		.tileMap = IMG_LoadTexture(pRenderer, "res/tile/tiles.png"),
		.locationFile = fopen(locationName, "r"),
		.rows = 12,
		.columns = 39,
		.locationDest.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE,
		.locationDest.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE,
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

void L_ObjectInit(obj_manager_t* pObjManager, int spriteIndex, int srcX, int srcY, int posX, int posY, bool isAnimated)
{
	pObjManager->animTimer[pObjManager->objCount].reactionTime = ANIM_TIME;
	pObjManager->isAnimated[pObjManager->objCount] = isAnimated;

	pObjManager->objDest.w = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;
	pObjManager->objDest.h = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;
	pObjManager->transforms[pObjManager->objCount].logX = (float) posX;
	pObjManager->transforms[pObjManager->objCount].logY = (float) posY;

	pObjManager->sprites[pObjManager->objCount].currentSprite = 0;
	pObjManager->sprites[pObjManager->objCount].spriteImg = obj_sprites[spriteIndex];
	pObjManager->sprites[pObjManager->objCount].spriteSrc.w = TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.h = TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.x = srcX;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.y = srcY;

	++pObjManager->objCount;
}

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

/*
 * Destructor method to clean up all location textures
 * (Always need to be called in application crash or normal exit!!!)
 */
void L_Destruct(location_t* pLocation, obj_manager_t* pObjManager)
{
	for (int i = 0; i < pObjManager->objCount; ++i)
		pObjManager->sprites[i].spriteImg = NULL;

	for (int i = 0; i < MAX_OBJ_SPRITES; ++i)
	{
		if (obj_sprites[i] != NULL)
			SDL_DestroyTexture(obj_sprites[i]);
	}

	if (pLocation->tileMap != NULL)
	{
		SDL_DestroyTexture(pLocation->tileMap);
		pLocation->tileMap = NULL;
	}
}
#endif /* STB_LOCATION_IMPLEMENTATION */

#endif /* L_LOCATION_H_ */
