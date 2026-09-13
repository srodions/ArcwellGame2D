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
		printf("[ARC_LOADER]::(ERR) Error loading assets header: 'The assets file is not opened for reading'\n");
		return NULL;
	}

	arcf_header_t* pHeader = malloc(sizeof(arcf_header_t));
	fread(pHeader, sizeof(arcf_header_t), 1, arcFile);

	const char* correctSignature = "ARCF";
	if (strncmp(pHeader->signature, correctSignature, 4) != 0)
	{
		printf("[ARC_LOADER]::(ERR) Error loading assets header: 'The assets file signature is not correct %s != %s'\n", pHeader->signature, correctSignature);
		return NULL;
	}

	printf("[ARC_LOADER]::(LOG) Assets header loaded successfully\n");
	return pHeader;
}

arcf_entry_t* L_LoadLumpsTable(FILE* arcFile, arcf_header_t* pHeader)
{
	if (!arcFile)
	{
		printf("[ARC_LOADER]::(ERR) Error loading lumps table: 'The assets file is not opened for reading'\n");
		return NULL;
	}

	fseek(arcFile, pHeader->offsetToLumpsTable, SEEK_SET);
	arcf_entry_t* pTable = (arcf_entry_t*) malloc(sizeof(arcf_entry_t) * pHeader->lumpsCount);
	fread(pTable, sizeof(arcf_entry_t), pHeader->lumpsCount, arcFile);

	if (!pTable)
		printf("[ARC_LOADER]::(ERR) Error loading lumps table: 'Incorrect lumps count %d (Corrupted assets file)'\n", pHeader->lumpsCount);

	printf("[ARC_LOADER]::(LOG) Lumps table loaded successfully\n");
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

			printf("[ARC_LOADER]::(LOG) Loading lump: '%s';\tSize: %d bytes\n", lumpName, *outSize);
			break;
		}
	}

	if (!buffer)
		printf("[ARC_LOADER]::(ERR) Error loading lump: '%s'\n", lumpName);

	return buffer;
}


