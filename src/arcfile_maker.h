#ifndef ARCFILE_MAKER_H_
#define ARCFILE_MAKER_H_

#include "typedefs.h"
#include <stdio.h>

extern arcf_entry_t allEntries[16];

void packMap_Tomb(FILE* arcFile, uint32_t rows, uint32_t columns, uint32_t* currentOffset, uint32_t* currentFilesCount);
void packFile(const char* filePath, const char* fileName, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
void packNamesHeader(FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
arcf_entcfgheader_t* initEntHeader(uint32_t entCfgsCount);
void fillEntityData(arcf_entcfgheader_t* entHeader, int afc, int afr, int asi, int atd, int atfc, int atfr,
int dfc, int dfr,int dmsi, int hfc, int hfr, int knck, int mhp, int pX, int pY, int sfc, int sfr, int spd,
int str, int wfc, int wfr);
void packEntityData(arcf_entcfgheader_t* entHeader, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
void packSpriteFile(const char* filePath, const char* fileName, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
void fillObjectData(arcf_objheader_t* objHeader, uint32_t si, uint32_t bsx, uint32_t bsy, uint32_t btx, uint32_t bty, bool isAnim);
void packObjectsData(arcf_objheader_t* objHeader, FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
void finishArchive(FILE* arcFile, uint32_t* currentOffset, uint32_t* currentFilesCount);
void* loadLump(const char* filePath, const char* lumpName, uint32_t* outSize);
void printMap(arcf_mapheader_t* mapDataHeader);
void printObjectData(arcf_objheader_t* objHeader);
arcf_objheader_t* initObjectsHeader(uint32_t objCount);

#endif /* ARCFILE_MAKER_H_ */
