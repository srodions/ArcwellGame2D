#ifndef L_LOCATION_H_
#define L_LOCATION_H_

/* --- DEFINITIONS --- */
location_t* L_LocationInit(const char* filePath, SDL_Renderer* pRenderer, obj_manager_t* pObjManager);
tile_t L_TileInit(int srcX, int srcY, int posX, int posY);
void L_ObjectInit(obj_manager_t* pObjManager, int spriteIndex, int bsx, int bsy, int btx, int bty, bool isAnim);
void L_ObjectSetter(obj_manager_t* pObjManager, const char* jsonFilePath);
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

void L_ObjectSetter(obj_manager_t* pObjManager, const char* jsonFilePath)
{
	char* data = (char*) SDL_LoadFile(jsonFilePath, NULL);

	cJSON *json = cJSON_Parse(data);

	if (json == NULL)
	{
		const char* errorMsg = cJSON_GetErrorPtr();
		if (errorMsg != NULL)
			fprintf(stderr, "JSON Parse Error: %s\n", errorMsg);
		SDL_free(data);
		return;
	}

	cJSON* objects = cJSON_GetObjectItem(json, "objects");
	cJSON* object = NULL;

	cJSON_ArrayForEach(object, objects)
	{
		int spriteIndex = cJSON_GetObjectItem(object, "spriteIndex")->valueint;
		int bsx = cJSON_GetObjectItem(object, "bySpriteX")->valueint;
		int bsy = cJSON_GetObjectItem(object, "bySpriteY")->valueint;
	    int btx = cJSON_GetObjectItem(object, "byTileX")->valueint;
	    int bty = cJSON_GetObjectItem(object, "byTileY")->valueint;
	    bool isAnim = cJSON_IsTrue(cJSON_GetObjectItem(object, "isAnimated"));

	    L_ObjectInit(pObjManager, spriteIndex, bsx, bsy, btx, bty, isAnim);
	}

	if (data != NULL)
		SDL_free(data);
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

/*
 * This new function is not compatible with Android platform
 * TODO: Use SDL file methods in future
 */
location_t* L_LocationInit(const char* filePath, SDL_Renderer* pRenderer, obj_manager_t* pObjManager)
{
	FILE* mapFile = fopen(filePath, "rb");
	if (!mapFile) return NULL;

	arcf_header_t header;
	fread(&header, sizeof(arcf_header_t), 1, mapFile);

	assert(header.rows <= MAX_MAP_ROWS && header.columns <= MAX_MAP_COLUMNS);

	if (strncmp(header.signature, "ARCF", 4) != 0)
	{
		printf("Missing signature!\n");
		fclose(mapFile);
		return NULL;
	}

	uint32_t rows = header.rows;
	uint32_t columns = header.columns;
	const int totalTiles = rows * columns;

	location_t* location = (location_t*) malloc(sizeof(location_t));
	location->tileMap = IMG_LoadTexture(pRenderer, "res/tile/tiles.png");
	location->rows = rows;
	location->columns = columns;
	location->locationDest.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	location->locationDest.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	location->locationTiles = (tile_t*) malloc(totalTiles * sizeof(tile_t));

	char* mapData = (char*) malloc(totalTiles * sizeof(char));

	fread(mapData, sizeof(char), totalTiles, mapFile);
	fclose(mapFile);

	printf("Map (%d * %d) loaded!\n", rows, columns);

	int tempY = 0;

	for (uint32_t y = 0; y < rows; ++y)
	{
		int srcX = 0;
		int srcY = 0;
		int tempX = 0;

		for (uint32_t x = 0; x < columns; ++x)
		{
			char currentTile = mapData[y * header.columns + x];

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

			location->locationTiles[y * header.columns + x] = L_TileInit(srcX, srcY, tempX, tempY);
			tempX += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
		}

		tempY += TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	}

	if (mapData != NULL) free(mapData);

	return location;
}

void L_ObjectInit(obj_manager_t* pObjManager, int spriteIndex, int bsx, int bsy, int btx, int bty, bool isAnim)
{
	assert(pObjManager->objCount <= MAX_OBJECTS);

	pObjManager->animTimer[pObjManager->objCount].reactionTime = ANIM_TIME;
	pObjManager->isAnimated[pObjManager->objCount] = isAnim;

	pObjManager->objDest.w = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;
	pObjManager->objDest.h = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;
	pObjManager->transforms[pObjManager->objCount].logX = (float) btx * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	pObjManager->transforms[pObjManager->objCount].logY = (float) bty * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;

	pObjManager->sprites[pObjManager->objCount].currentSprite = 0;
	pObjManager->sprites[pObjManager->objCount].spriteImg = obj_sprites[spriteIndex];
	pObjManager->sprites[pObjManager->objCount].spriteSrc.w = TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.h = TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.x = bsx * TILE_SPRITE_SIZE;
	pObjManager->sprites[pObjManager->objCount].spriteSrc.y = bsy * TILE_SPRITE_SIZE;

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
	if (pLocation->tileMap != NULL)
	{
	    SDL_DestroyTexture(pLocation->tileMap);
		pLocation->tileMap = NULL;
	}

	for (int i = 0; i < MAX_OBJ_SPRITES; ++i)
	{
		if (obj_sprites[i] != NULL)
		{
			SDL_DestroyTexture(obj_sprites[i]);
			obj_sprites[i] = NULL;
		}
	}

	for (int i = 0; i < pObjManager->objCount; ++i)
		pObjManager->sprites[i].spriteImg = NULL;
}
#endif /* STB_LOCATION_IMPLEMENTATION */

#endif /* L_LOCATION_H_ */
