#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#define SPRITE_SCALE 2
#include <SDL_image.h>
#include <SDL.h>

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

arcf_entry_t allEntries[16];

void packMap_Tomb(FILE* arcFile, uint32_t rows, uint32_t columns, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
	const char* design[] = {
		"........................................",
		"........................................",
		"IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII",
		"........................................",
		"........................................",
		"........................................",
		"........................................",
		"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
		"IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII"
	};

	int maxRows = sizeof(design) / sizeof(design[0]);
	if (rows > maxRows) rows = maxRows;

	const int totalTiles = rows * columns;
	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) malloc(totalTiles + sizeof(arcf_mapheader_t));
	mapDataHeader->mapRows = rows;
	mapDataHeader->mapColumns = columns;
	char* mapData = (char*)(mapDataHeader + 1);

	// Filling map data array string by string from design array
	for(int y = 0; y < rows; y++)
	    memcpy(&mapData[y * columns], design[y], columns);

	allEntries[*currentFilesCount].offsetToFile = *currentOffset;
	allEntries[*currentFilesCount].lumpSize = totalTiles + sizeof(arcf_mapheader_t);
	snprintf(allEntries[*currentFilesCount].lumpName, 16, "TOMB");

	fseek(arcFile, *currentOffset, SEEK_SET);
	fwrite(mapDataHeader, sizeof(arcf_mapheader_t), 1, arcFile);
	fwrite(mapData, sizeof(char), totalTiles, arcFile);

	*currentOffset += (sizeof(char) * totalTiles + sizeof(arcf_mapheader_t));
	++(*currentFilesCount);

	free(mapDataHeader);
	printf("Map (%d * %d) packed at offset %u!\n", rows, columns, *currentOffset);
}

void packTexture(const char* filePath, const char* textureName, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
	FILE* imgFile = fopen(filePath, "rb");
	if (!imgFile) return;

	// Finding image size
	fseek(imgFile, 0, SEEK_END);
	long imgSize = ftell(imgFile);
	fseek(imgFile, 0, SEEK_SET);

	unsigned char* imgBytesBuffer = (unsigned char*) malloc(imgSize);
	fread(imgBytesBuffer, 1, imgSize, imgFile);
	fclose(imgFile);

	allEntries[*currentFilesCount].offsetToFile = *currentOffset;
	allEntries[*currentFilesCount].lumpSize = (uint32_t)imgSize;
	memset(allEntries[*currentFilesCount].lumpName, 0, sizeof(allEntries[*currentFilesCount].lumpName));
	strncpy(allEntries[*currentFilesCount].lumpName, textureName, 15);

	fseek(arcFile, *currentOffset, SEEK_SET);
	fwrite(imgBytesBuffer, 1, imgSize, arcFile);

	*currentOffset += (uint32_t)imgSize;
	++(*currentFilesCount);

	free(imgBytesBuffer);
	printf("Texture '%s' (%ld bytes) packed at offset %u!\n", textureName, imgSize, *currentOffset);
}

arcf_objheader_t* initObjectsHeader(uint32_t objCount)
{
	arcf_objheader_t* objHeader = (arcf_objheader_t*) malloc(sizeof(arcf_header_t) + sizeof(arcf_objentry_t) * objCount);
	objHeader->objCount = 0;
	return objHeader;
}

void fillObjectData(arcf_objheader_t* objHeader, uint32_t si, uint32_t bsx, uint32_t bsy, uint32_t btx, uint32_t bty, bool isAnim)
{
	arcf_objentry_t obj = {
		.spriteIndex = si,
		.bySpriteX = bsx,
		.bySpriteY = bsy,
		.byTileX = btx,
		.byTileY = bty,
		.isAnimated = isAnim
	};

	objHeader->items[objHeader->objCount] = obj;
	++(objHeader->objCount);
}

void packObjectsData(arcf_objheader_t* objHeader, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
	uint32_t objCount = objHeader->objCount;
	uint32_t totalSize = sizeof(arcf_objheader_t) + sizeof(arcf_objentry_t) * objCount;

	allEntries[*currentFilesCount].offsetToFile = *currentOffset;
	allEntries[*currentFilesCount].lumpSize = totalSize;
	memset(allEntries[*currentFilesCount].lumpName, 0, sizeof(allEntries[*currentFilesCount].lumpName));
	strncpy(allEntries[*currentFilesCount].lumpName, "OBJECTS", 15);

	fseek(arcFile, *currentOffset, SEEK_SET);
	fwrite(objHeader, 1, totalSize, arcFile);

	*currentOffset += totalSize;
	++(*currentFilesCount);

	printf("Objects data (%d bytes) packed at offset %u!\n", totalSize, *currentOffset);
	free(objHeader);
}

void finishArchive(FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
    uint32_t tableOffset = *currentOffset;

    fseek(arcFile, tableOffset, SEEK_SET);
    fwrite(allEntries, sizeof(arcf_entry_t), *currentFilesCount, arcFile);

    arcf_header_t header;
    memcpy(header.signature, "ARCF", 4);
    header.lumpsCount = *currentFilesCount;
    header.offsetToLumpsTable = tableOffset;

    fseek(arcFile, 0, SEEK_SET);
    fwrite(&header, sizeof(arcf_header_t), 1, arcFile);

    printf("\nArchive finished! Table at: %u, Files: %u\n", tableOffset, *currentFilesCount);
}

void* loadLump(const char* filePath, const char* lumpName, uint32_t* outSize)
{
	FILE* arcFile = fopen(filePath, "rb");
	if (!arcFile) return NULL;

	arcf_header_t header;
	fread(&header, sizeof(arcf_header_t), 1, arcFile);

	if (strncmp(header.signature, "ARCF", 4) != 0)
	{
		printf("Missing signature!\n");
		fclose(arcFile);
		return NULL;
	}

	fseek(arcFile, header.offsetToLumpsTable, SEEK_SET);
	arcf_entry_t* table = malloc(sizeof(arcf_entry_t) * header.lumpsCount);
	fread(table, sizeof(arcf_entry_t), header.lumpsCount, arcFile);

	void* buffer = NULL;
	for (uint32_t i = 0; i < header.lumpsCount; ++i) {
		if (strcmp(table[i].lumpName, lumpName) == 0) {

			buffer = malloc(table[i].lumpSize);
			fseek(arcFile, table[i].offsetToFile, SEEK_SET);
			fread(buffer, 1, table[i].lumpSize, arcFile);

			if (outSize) *outSize = table[i].lumpSize;
			break;
		}
	}

	free(table);
	fclose(arcFile);
	printf("\nLump '%s' (%d bytes) loaded!\n", lumpName, *outSize);

	return buffer;
}

void printMap(arcf_mapheader_t* mapDataHeader)
{
	if (!mapDataHeader) return;

	uint32_t rows = mapDataHeader->mapRows;
	uint32_t columns = mapDataHeader->mapColumns;
	char* mapData = (char*)(mapDataHeader + 1);

	printf("Preview (%d * %d):\n", rows, columns);

	for (uint32_t y = 0; y < rows; ++y)
	{
		for (uint32_t x = 0; x < columns; ++x)
		{
			char currentTile = mapData[y * columns + x];
			printf("%c", currentTile);
		}

		printf("\n");
	}
}

void printObjectData(arcf_objheader_t* objHeader)
{
	uint32_t objCount = objHeader->objCount;

	printf("Objects Data Preview:\n");
	for (int i = 0; i < objCount; ++i)
	{
		printf("spriteIndex: %d\n", objHeader->items[i].spriteIndex);
		printf("bySpriteX: %d\n", objHeader->items[i].bySpriteX);
		printf("bySpriteY: %d\n", objHeader->items[i].bySpriteY);
		printf("byTileX: %d\n", objHeader->items[i].byTileX);
		printf("byTileY: %d\n", objHeader->items[i].byTileY);
		printf("isAnimated: %d\n", objHeader->items[i].isAnimated);
		printf("===============\n");
	}
}

void displayTexture(SDL_Renderer* pRenderer, void* textureData, uint32_t* currentFileSize, int width, int height)
{
	SDL_Texture* texture;

	if (textureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(textureData, *currentFileSize);
		texture = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(textureData);
	}

	SDL_Rect destRect = {
		.x = 0,
		.y = 0,
		.w = width,
		.h = height
	};
	SDL_Event event;
	bool running = true;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = false;
		}

		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
		SDL_RenderClear(pRenderer);
		SDL_RenderCopy(pRenderer, texture, NULL, &destRect);
		SDL_RenderPresent(pRenderer);

		SDL_Delay(16);
	}

	SDL_DestroyTexture(texture);
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	SDL_Window* pWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

	uint32_t currentOffset = sizeof(arcf_header_t);
	uint32_t currentFilesCount = 0;
	uint32_t currentFileSize = 0;

	FILE* arcFile = fopen("out/assets.arc", "wb");

	packTexture("in/player.png", "PLAYER", arcFile, &currentOffset, &currentFilesCount);
	packTexture("in/skeleton.png", "SKELETON", arcFile, &currentOffset, &currentFilesCount);
	packTexture("in/tiles.png", "TILES", arcFile, &currentOffset, &currentFilesCount);
	packTexture("in/torch.png", "TORCH", arcFile, &currentOffset, &currentFilesCount);
	packTexture("in/chest.png", "CHEST", arcFile, &currentOffset, &currentFilesCount);
	packTexture("in/decoration.png", "DECORATION", arcFile, &currentOffset, &currentFilesCount);

	packMap_Tomb(arcFile, 9, 39, &currentOffset, &currentFilesCount);

	arcf_objheader_t* objs = initObjectsHeader(22);
	// TORCH 1
	fillObjectData(objs, 0, 0, 0, 3, 4, true);
	// TORCH 2
	fillObjectData(objs, 0, 0, 0, 11, 4, true);
	// TORCH 3
	fillObjectData(objs, 0, 0, 0, 19, 4, true);
	// TORCH 4
	fillObjectData(objs, 0, 0, 0, 27, 4, true);
	// TORCH 5
	fillObjectData(objs, 0, 0, 0, 35, 4, true);
	// CHEST
	fillObjectData(objs, 2, 0, 0, 11, 6, false);
	// COLUMN 1
	fillObjectData(objs, 1, 1, 1, 7, 6, false);
	fillObjectData(objs, 1, 2, 1, 7, 5, false);
	fillObjectData(objs, 1, 2, 1, 7, 4, false);
	fillObjectData(objs, 1, 3, 1, 7, 3, false);
	// COLUMN 2
	fillObjectData(objs, 1, 1, 1, 15, 6, false);
	fillObjectData(objs, 1, 2, 1, 15, 5, false);
	fillObjectData(objs, 1, 2, 1, 15, 4, false);
	fillObjectData(objs, 1, 3, 1, 15, 3, false);
	// COLUMN 3
	fillObjectData(objs, 1, 1, 1, 23, 6, false);
	fillObjectData(objs, 1, 2, 1, 23, 5, false);
	fillObjectData(objs, 1, 2, 1, 23, 4, false);
	fillObjectData(objs, 1, 3, 1, 23, 3, false);
	// COLUMN 4
	fillObjectData(objs, 1, 1, 1, 31, 6, false);
	fillObjectData(objs, 1, 2, 1, 31, 5, false);
	fillObjectData(objs, 1, 2, 1, 31, 4, false);
	fillObjectData(objs, 1, 3, 1, 31, 3, false);
	packObjectsData(objs, arcFile, &currentOffset, &currentFilesCount);

	finishArchive(arcFile, &currentOffset, &currentFilesCount);

	fclose(arcFile);

	void* playerTextureData = loadLump("out/assets.arc", "PLAYER", &currentFileSize);
	displayTexture(pRenderer, playerTextureData, &currentFileSize, 256 * SPRITE_SCALE, 32 * SPRITE_SCALE);

	void* skeletonTextureData = loadLump("out/assets.arc", "SKELETON", &currentFileSize);
	displayTexture(pRenderer, skeletonTextureData, &currentFileSize, 320 * SPRITE_SCALE, 64 * SPRITE_SCALE);

	void* tilesTextureData = loadLump("out/assets.arc", "TILES", &currentFileSize);
	displayTexture(pRenderer, tilesTextureData, &currentFileSize, 80 * SPRITE_SCALE, 32 * SPRITE_SCALE);

	void* torchTextureData = loadLump("out/assets.arc", "TORCH", &currentFileSize);
	displayTexture(pRenderer, torchTextureData, &currentFileSize, 80 * SPRITE_SCALE, 16 * SPRITE_SCALE);

	void* chestTextureData = loadLump("out/assets.arc", "CHEST", &currentFileSize);
	displayTexture(pRenderer, chestTextureData, &currentFileSize, 48 * SPRITE_SCALE, 16 * SPRITE_SCALE);

	void* decorationTextureData = loadLump("out/assets.arc", "DECORATION", &currentFileSize);
	displayTexture(pRenderer, decorationTextureData, &currentFileSize, 80 * SPRITE_SCALE, 32 * SPRITE_SCALE);

	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) loadLump("out/assets.arc", "TOMB", &currentFileSize);
	printMap(mapDataHeader);
	free(mapDataHeader);

	arcf_objheader_t* objDataHeader = (arcf_objheader_t*) loadLump("out/assets.arc", "OBJECTS", &currentFileSize);
	printObjectData(objDataHeader);
	free(objDataHeader);

	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}


