#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "l_arcloader.h"
#include "typedefs.h"

arcf_header_t* L_LoadHeader(FILE* arcFile)
{
	if (!arcFile)
	{
		printf("[ARC_LOADER] Error loading assets file: The file is not opened for reading\n");
		return NULL;
	}

	arcf_header_t* pHeader = malloc(sizeof(arcf_header_t));
	fread(pHeader, sizeof(arcf_header_t), 1, arcFile);

	if (strncmp(pHeader->signature, "ARCF", 4) != 0)
	{
		printf("[ARC_LOADER] Error loading assets file: The file signature is not correct\n");
		return NULL;
	}

	printf("[ARC_LOADER] Assets file loaded successfully\n");
	return pHeader;
}

arcf_entry_t* L_LoadLumpsTable(FILE* arcFile, arcf_header_t* pHeader)
{
	if (!arcFile) return NULL;

	fseek(arcFile, pHeader->offsetToLumpsTable, SEEK_SET);
	arcf_entry_t* pTable = (arcf_entry_t*) malloc(sizeof(arcf_entry_t) * pHeader->lumpsCount);
	fread(pTable, sizeof(arcf_entry_t), pHeader->lumpsCount, arcFile);

	return pTable;
}

void* L_LoadLump(FILE* arcFile, const char* lumpName, arcf_header_t* pHeader, arcf_entry_t* pTable, uint32_t* outSize)
{
	if (!arcFile) return NULL;

	void* buffer = NULL;
	for (uint32_t i = 0; i < pHeader->lumpsCount; ++i)
	{
		if (strcmp(pTable[i].lumpName, lumpName) == 0)
		{
			buffer = malloc(pTable[i].lumpSize);
			fseek(arcFile, pTable[i].offsetToFile, SEEK_SET);
			fread(buffer, 1, pTable[i].lumpSize, arcFile);

			if (outSize) *outSize = pTable[i].lumpSize;
			break;
		}
	}

	return buffer;
}


