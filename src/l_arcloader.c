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
	if (!pHeader)
	{
		printf("[ARC_LOADER]::(ERR) Error allocating memory for ARCF header\n");
		return NULL;
	}

	size_t readCount = fread(pHeader, sizeof(arcf_header_t), 1, arcFile);
	if (readCount != 1)
	{
		printf("[ARC_LOADER]::(ERR) Error reading assets file: 'Unexpected end of file while reading header'\n");
		free(pHeader);
		return NULL;
	}

	const char* correctSignature = "ARCF";
	if (strncmp(pHeader->signature, correctSignature, 4) != 0)
	{
		printf("[ARC_LOADER]::(ERR) Error loading assets header: 'The assets file signature is not correct %s != %s'\n", pHeader->signature, correctSignature);
		free(pHeader);
		return NULL;
	}

	printf("[ARC_LOADER]::(LOG) Assets header loaded successfully\n");
	return pHeader;
}

arcf_entry_t* L_LoadLumpsTable(FILE* arcFile, arcf_header_t* pHeader)
{
	if (!arcFile || !pHeader) return NULL;

	if (pHeader->lumpsCount == 0)
	{
		printf("[ARC_LOADER]::(ERR) Lumps table is empty (lumpsCount == 0)\n");
		return NULL;
	}

	if (fseek(arcFile, (long) pHeader->offsetToLumpsTable, SEEK_SET) != 0)
	{
		printf("[ARC_LOADER]::(ERR) Error seeking to lumps table offset\n");
		return NULL;
	}

	arcf_entry_t* pTable = (arcf_entry_t*) malloc(sizeof(arcf_entry_t) * pHeader->lumpsCount);
	if (!pTable)
	{
		printf("[ARC_LOADER]::(ERR) Error allocating memory for lumps table\n");
		return NULL;
	}

	size_t readCount = fread(pTable, sizeof(arcf_entry_t), pHeader->lumpsCount, arcFile);
	if (readCount != pHeader->lumpsCount)
	{
		printf("[ARC_LOADER]::(ERR) Error reading lumps table: 'Unexpected end of file'\n");
		free(pTable);
		return NULL;
	}

	// Force NUL-termination on every lump name, in case the file was
	// authored/copied without a trailing zero byte on a full 16-char name.
	for (uint32_t i = 0; i < pHeader->lumpsCount; ++i)
		pTable[i].lumpName[sizeof(pTable[i].lumpName) - 1] = '\0';

	printf("[ARC_LOADER]::(LOG) Lumps table loaded successfully\n");
	return pTable;
}

void* L_LoadLump(FILE* arcFile, const char* lumpName, arcf_header_t* pHeader, arcf_entry_t* pTable, uint32_t* outSize)
{
	if (outSize) *outSize = 0;

	if (!arcFile || !pHeader || !pTable || !lumpName) return NULL;

	void* buffer = NULL;

	for (uint32_t i = 0; i < pHeader->lumpsCount; ++i)
	{
		if (strncmp(pTable[i].lumpName, lumpName, sizeof(pTable[i].lumpName)) == 0)
		{
			if (pTable[i].lumpSize == 0)
			{
				printf("[ARC_LOADER]::(ERR) Lump '%s' has zero size\n", lumpName);
				return NULL;
			}

			buffer = malloc(pTable[i].lumpSize);
			if (!buffer)
			{
				printf("[ARC_LOADER]::(ERR) Error allocating %u bytes for lump: '%s'\n", pTable[i].lumpSize, lumpName);
				return NULL;
			}

			if (fseek(arcFile, (long) pTable[i].offsetToFile, SEEK_SET) != 0)
			{
				printf("[ARC_LOADER]::(ERR) Error seeking to lump: '%s'\n", lumpName);
				free(buffer);
				return NULL;
			}

			size_t readCount = fread(buffer, 1, pTable[i].lumpSize, arcFile);
			if (readCount != pTable[i].lumpSize)
			{
				printf("[ARC_LOADER]::(ERR) Error reading lump '%s': 'Unexpected end of file'\n", lumpName);
				free(buffer);
				return NULL;
			}

			if (outSize) *outSize = pTable[i].lumpSize;

			printf("[ARC_LOADER]::(LOG) Loading lump: '%s';\tSize: %u bytes\n", lumpName, pTable[i].lumpSize);
			return buffer;
		}
	}

	printf("[ARC_LOADER]::(ERR) Error loading lump: '%s'\n", lumpName);
	return NULL;
}
