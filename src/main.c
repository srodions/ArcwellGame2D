#include "arcfile_maker.h"
#include <stdlib.h>

int main()
{
	uint32_t currentOffset = sizeof(arcf_header_t);
	uint32_t currentFilesCount = 0;

	FILE* arcFile = fopen("out/assets.arc", "wb");

	packNamesHeader(arcFile, &currentOffset, &currentFilesCount);

	arcf_objheader_t* objs = initObjectsHeader(42);
	// COLUMN 1
	fillObjectData(objs, 1, 1, 1, 5, 11, false);
	fillObjectData(objs, 1, 2, 1, 5, 10, false);
	fillObjectData(objs, 1, 3, 1, 5, 9, false);
	// COLUMN 2
	fillObjectData(objs, 1, 1, 1, 7, 11, false);
	fillObjectData(objs, 1, 2, 1, 7, 10, false);
	fillObjectData(objs, 1, 3, 1, 7, 9, false);
	// COLUMN 3
	fillObjectData(objs, 1, 1, 1, 9, 11, false);
	fillObjectData(objs, 1, 2, 1, 9, 10, false);
	fillObjectData(objs, 1, 3, 1, 9, 9, false);
	// COLUMN 4
	fillObjectData(objs, 1, 1, 1, 11, 11, false);
	fillObjectData(objs, 1, 2, 1, 11, 10, false);
	fillObjectData(objs, 1, 3, 1, 11, 9, false);
	// COLUMN 5
	fillObjectData(objs, 1, 1, 1, 13, 11, false);
	fillObjectData(objs, 1, 2, 1, 13, 10, false);
	fillObjectData(objs, 1, 3, 1, 13, 9, false);
	// COLUMN 6
	fillObjectData(objs, 1, 1, 1, 15, 11, false);
	fillObjectData(objs, 1, 2, 1, 15, 10, false);
	fillObjectData(objs, 1, 3, 1, 15, 9, false);
	// COLUMN 7
	fillObjectData(objs, 1, 1, 1, 17, 11, false);
	fillObjectData(objs, 1, 2, 1, 17, 10, false);
	fillObjectData(objs, 1, 2, 1, 17, 9, false);
	fillObjectData(objs, 1, 2, 1, 17, 8, false);
	fillObjectData(objs, 1, 2, 1, 17, 7, false);
	fillObjectData(objs, 1, 2, 1, 17, 6, false);
	fillObjectData(objs, 1, 3, 1, 17, 5, false);
	// COLUMN 8
	fillObjectData(objs, 1, 1, 1, 23, 11, false);
	fillObjectData(objs, 1, 3, 1, 23, 10, false);
	// COLUMN 9
	fillObjectData(objs, 1, 1, 1, 34, 11, false);
	fillObjectData(objs, 1, 2, 1, 34, 10, false);
	fillObjectData(objs, 1, 2, 1, 34, 9, false);
	fillObjectData(objs, 1, 2, 1, 34, 8, false);
	fillObjectData(objs, 1, 2, 1, 34, 7, false);
	fillObjectData(objs, 1, 2, 1, 34, 6, false);
	fillObjectData(objs, 1, 3, 1, 34, 5, false);
	// LIANA 1
	fillObjectData(objs, 1, 4, 1, 5, 10, false);
	// LIANA 2
	fillObjectData(objs, 1, 4, 1, 7, 11, false);
	// LIANA 3
	fillObjectData(objs, 1, 4, 1, 9, 10, false);
	// LIANA 4
	fillObjectData(objs, 1, 4, 1, 15, 10, false);
	// TORCH 1
	fillObjectData(objs, 0, 0, 0, 17, 6, true);
	// TORCH 2
	fillObjectData(objs, 0, 0, 0, 17, 8, true);
	// TORCH 3
	fillObjectData(objs, 0, 0, 0, 34, 6, true);
	// TORCH 4
	fillObjectData(objs, 0, 0, 0, 34, 8, true);
	packObjectsData(objs, arcFile, &currentOffset, &currentFilesCount);

	arcf_entcfgheader_t* ents = initEntHeader(4);
	fillEntityData(ents, 0, 0, 0, 24, 4, 2, 0, 0, 1, 2, 20, 6, 100, 160, 0, 0, 20, 2, 8, 1, 6, 0);
	fillEntityData(ents, 11, 4, 1, 24, 9, 5, 7, 3, 3, 3, 20, 6, 384, 160, 11, 2, 16, 1, 8, 1, 8, 0);
	fillEntityData(ents, 11, 4, 1, 24, 9, 5, 7, 3, 3, 3, 20, 6, 416, 160, 11, 2, 16, 1, 8, 1, 8, 0);
	fillEntityData(ents, 11, 4, 1, 24, 9, 5, 7, 3, 3, 3, 20, 6, 432, 160, 11, 2, 16, 1, 8, 1, 8, 0);
	packEntityData(ents, arcFile, &currentOffset, &currentFilesCount);

	packMap_Tomb(arcFile, 19, 36, 15, 20, &currentOffset, &currentFilesCount);

	packSpriteFile("in/font_bitmap.png", "FNT", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/health_bar.png", "HLTHBR", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/player.png", "PLR", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/skeleton.png", "SKLTN", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/tiles.png", "TMBTLS", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/bg_cemetery.png", "BGCMTRY", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/torch.png", "TRCH", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/chest.png", "CHST", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/decoration.png", "DCRTN", arcFile, &currentOffset, &currentFilesCount);

	finishArchive(arcFile, &currentOffset, &currentFilesCount);

	fclose(arcFile);

	uint32_t currentFileSize = 0;

	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) loadLump("out/assets.arc", "TOMB", &currentFileSize);
	printMap(mapDataHeader);
	free(mapDataHeader);

	arcf_objheader_t* objDataHeader = (arcf_objheader_t*) loadLump("out/assets.arc", "OBJECTS", &currentFileSize);
	printObjectData(objDataHeader);
	free(objDataHeader);

	return 0;
}

