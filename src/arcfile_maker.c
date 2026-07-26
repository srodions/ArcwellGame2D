#include "arcfile_maker.h"
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void packFile(const char* filePath, const char* fileName, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
	FILE* imgFile = fopen(filePath, "rb");
	if (!imgFile) return;

	// Finding file size
	fseek(imgFile, 0, SEEK_END);
	long imgSize = ftell(imgFile);
	fseek(imgFile, 0, SEEK_SET);

	unsigned char* imgBytesBuffer = (unsigned char*) malloc(imgSize);
	fread(imgBytesBuffer, 1, imgSize, imgFile);
	fclose(imgFile);

	allEntries[*currentFilesCount].offsetToFile = *currentOffset;
	allEntries[*currentFilesCount].lumpSize = (uint32_t)imgSize;
	memset(allEntries[*currentFilesCount].lumpName, 0, sizeof(allEntries[*currentFilesCount].lumpName));
	strncpy(allEntries[*currentFilesCount].lumpName, fileName, 15);

	fseek(arcFile, *currentOffset, SEEK_SET);
	fwrite(imgBytesBuffer, 1, imgSize, arcFile);

	*currentOffset += (uint32_t)imgSize;
	++(*currentFilesCount);

	free(imgBytesBuffer);
	printf("File '%s' (%ld bytes) packed at offset %u!\n", fileName, imgSize, *currentOffset);
}

void packSpriteFile(const char* filePath, const char* fileName, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount)
{
    int imgW, imgH, channels;

    unsigned char* rawPixels = stbi_load(filePath, &imgW, &imgH, &channels, 4);

    if (!rawPixels) {
        printf("Decode error: %s\n", filePath);
        return;
    }

    uint32_t pixelDataSize = imgW * imgH * 4;
    uint32_t totalSize = pixelDataSize + sizeof(arcf_spriteheader_t);

    arcf_spriteheader_t* header = malloc(totalSize);
    header->spriteW = imgW;
    header->spriteH = imgH;

    unsigned char* imgBytesBuffer = (unsigned char*)(header + 1);

    uint32_t* outPixels = (uint32_t*)imgBytesBuffer;

    for (uint32_t i = 0; i < imgW * imgH; ++i) {
        uint8_t r = rawPixels[i * 4 + 0];
        uint8_t g = rawPixels[i * 4 + 1];
        uint8_t b = rawPixels[i * 4 + 2];
        uint8_t a = rawPixels[i * 4 + 3];

        outPixels[i] = (a << 24) | (b << 16) | (g << 8) | r;
    }

    stbi_image_free(rawPixels);

    allEntries[*currentFilesCount].offsetToFile = *currentOffset;
    allEntries[*currentFilesCount].lumpSize = totalSize;
    memset(allEntries[*currentFilesCount].lumpName, 0, sizeof(allEntries[*currentFilesCount].lumpName));
    strncpy(allEntries[*currentFilesCount].lumpName, fileName, 15);
    allEntries[*currentFilesCount].lumpName[15] = '\0';

    fseek(arcFile, *currentOffset, SEEK_SET);
    fwrite(header, 1, totalSize, arcFile);

    *currentOffset += totalSize;
    ++(*currentFilesCount);

    free(header);
    printf("File '%s' (%dx%d, %u bytes) packed successfully!\n", fileName, imgW, imgH, totalSize);
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
