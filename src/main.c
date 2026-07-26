#include "arcfile_maker.h"

int main()
{
	uint32_t currentOffset = sizeof(arcf_header_t);
	uint32_t currentFilesCount = 0;

	FILE* arcFile = fopen("out/assets.arc", "wb");

	packSpriteFile("in/font_bitmap.png", "FONT", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/player.png", "PLAYER", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/skeleton.png", "SKELETON", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/tiles.png", "TILES", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/torch.png", "TORCH", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/chest.png", "CHEST", arcFile, &currentOffset, &currentFilesCount);
	packSpriteFile("in/decoration.png", "DECORATION", arcFile, &currentOffset, &currentFilesCount);

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

	uint32_t currentFileSize = 0;

	arcf_mapheader_t* mapDataHeader = (arcf_mapheader_t*) loadLump("out/assets.arc", "TOMB", &currentFileSize);
	printMap(mapDataHeader);
	free(mapDataHeader);

	arcf_objheader_t* objDataHeader = (arcf_objheader_t*) loadLump("out/assets.arc", "OBJECTS", &currentFileSize);
	printObjectData(objDataHeader);
	free(objDataHeader);

	return 0;
}

