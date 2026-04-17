#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct ARCF_Header
{
	char signature[4]; 		// ARCF
	uint32_t rows;			// Map rows count
	uint32_t columns;		// Map columns count
} arcf_header_t;

void createMap(const char* filePath, uint32_t rows, uint32_t columns)
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
	assert(rows <= maxRows);

	// Filling header data
	arcf_header_t header;
	memcpy(header.signature, "ARCF", 4);
	header.rows = rows;
	header.columns = columns;

	const int totalTiles = rows * columns;
	char mapData[totalTiles];

	// Filling map data array string by string from design array
	for(int y = 0; y < rows; y++)
	    memcpy(&mapData[y * columns], design[y], columns);

	FILE* mapFile = fopen(filePath, "wb");
	if (!mapFile) return;

	fwrite(&header, sizeof(arcf_header_t), 1, mapFile);
	fwrite(mapData, sizeof(char), totalTiles, mapFile);
	fclose(mapFile);

	printf("Map (%d * %d) created!\n", rows, columns);
}

char* loadMap(const char* filePath)
{
	FILE* mapFile = fopen(filePath, "rb");
	if (!mapFile) return NULL;

	arcf_header_t header;
	fread(&header, sizeof(arcf_header_t), 1, mapFile);

	if (strncmp(header.signature, "ARCF", 4) != 0)
	{
		printf("Missing signature!\n");
		fclose(mapFile);
		return NULL;
	}

	const int totalTiles = header.rows * header.columns;
	char* mapData = (char*) malloc(totalTiles * sizeof(char));

	fread(mapData, sizeof(char), totalTiles, mapFile);
	fclose(mapFile);

	printf("Map (%d * %d) loaded!\n", header.rows, header.columns);

	return mapData;
}

void printMap(uint32_t rows, uint32_t columns, char* mapData)
{
	if (!mapData) return;

	printf("Preview:\n");

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

int main()
{
	char* mapData = NULL;

	createMap("res/tomb.arc", 9, 39);
	mapData = loadMap("res/tomb.arc");
	printMap(9, 39, mapData);

	return 0;
}


