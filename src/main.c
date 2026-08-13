#include "arcfile_maker.h"
#include <stdlib.h>

int main()
{
	uint32_t currentOffset = sizeof(arcf_header_t);
	uint32_t currentFilesCount = 0;

	FILE* arcFile = fopen("out/assets.arc", "wb");

	packNamesHeader(arcFile, &currentOffset, &currentFilesCount);

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

	arcf_entcfgheader_t* ents = initEntHeader(6);
	fillEntityData(ents, 0, 0, 0, 24, 4, 1, 0, 0, 2, 0, 0, 20, 6, 100, 100, 0, 0, 20, 2, 8, 0);
	fillEntityData(ents, 11, 3, 1, 24, 9, 4, 7, 2, 3, 0, 0, 20, 6, 180, 100, 11, 1, 16, 1, 8, 0);
	fillEntityData(ents, 11, 3, 1, 24, 9, 4, 7, 2, 3, 0, 0, 20, 6, 220, 100, 11, 1, 16, 1, 8, 0);
	fillEntityData(ents, 11, 3, 1, 24, 9, 4, 7, 2, 3, 0, 0, 20, 6, 240, 100, 11, 1, 16, 1, 8, 0);
	fillEntityData(ents, 11, 3, 1, 24, 9, 4, 7, 2, 3, 0, 0, 20, 6, 270, 100, 11, 1, 16, 1, 8, 0);
	fillEntityData(ents, 11, 3, 1, 24, 9, 4, 7, 2, 3, 0, 0, 20, 6, 280, 100, 11, 1, 16, 1, 8, 0);
	packEntityData(ents, arcFile, &currentOffset, &currentFilesCount);

	packMap_Tomb(arcFile, 9, 39, &currentOffset, &currentFilesCount);

	packSpriteFile("in/font_bitmap.png", "FNT", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/health_bar.png", "HLTHBR", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/player.png", "PLR", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/skeleton.png", "SKLTN", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/tiles.png", "TMBTLS", arcFile, &currentOffset, &currentFilesCount);
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

