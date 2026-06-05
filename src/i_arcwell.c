#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "g_entity.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "g_gamestate.h"
#include "g_map.h"
#include "h_keyboard.h"
#include "i_system.h"
#include "r_renderer.h"
#include "typedefs.h"

gamestate_t gameState;
map_t* map;
obj_manager_t objManager;
e_manager_t entManager;
rtimer_t spawnTimer;

int I_GameInit()
{
	if (I_LibInit() < 0) return -1;

	I_InitKeymap();
	I_InitBtnMap();
	gameState = G_GameInit();

	if (I_WindowInit(&gameState) < 0 || I_RendererInit() < 0) return -1;

	I_FontInit("res/font/x12y16pxMaruMonica.ttf", 40);
	entManager.entitiesCount = 0;
	spawnTimer.reactionTime = ENTITY_SPAWN_TIME;
	objManager.objCount = 0;

	FILE* arcFile = fopen("res/assets.arc", "rb");
	if (!arcFile) return -1;
	arcf_header_t* header = L_LoadHeader(arcFile);
	if (!header) return -1;
	arcf_entry_t* table = L_LoadLumpsTable(arcFile, header);
	if (!table) return -1;

	uint32_t currentTextureSize = 0;
	void* tileMapTextureData = L_LoadLump(arcFile, "TILES", header, table, &currentTextureSize);
	I_InitTilemapTextureFromData(tileMapTextureData, currentTextureSize);
	void* playerTextureData = L_LoadLump(arcFile, "PLAYER", header, table, &currentTextureSize);
	I_InitEntityTextureFromData(playerTextureData, currentTextureSize, PLAYER);
	void* skeletonTextureData = L_LoadLump(arcFile, "SKELETON", header,  table, &currentTextureSize);
	I_InitEntityTextureFromData(skeletonTextureData, currentTextureSize, SKELETON);
	void* torchTextureData = L_LoadLump(arcFile, "TORCH", header, table, &currentTextureSize);
	I_InitObjTextureFromData(torchTextureData, currentTextureSize, TORCH);
	void* decorationTextureData = L_LoadLump(arcFile, "DECORATION", header, table, &currentTextureSize);
	I_InitObjTextureFromData(decorationTextureData, currentTextureSize, DECORATION);
	void* chestTextureData = L_LoadLump(arcFile, "CHEST", header, table, &currentTextureSize);
	I_InitObjTextureFromData(chestTextureData, currentTextureSize, CHEST);

	map = G_MapInit(arcFile, header, table, &objManager);
	G_ObjSetter(arcFile, header, table, &objManager);
	fclose(arcFile);
	free(header);
	free(table);

	G_EntityInit(&entManager, 1050, FLOOR_DISTANCE, player_speed, attack_knockback, PLAYER_HP, PLAYER_STRENGTH, PLAYER); // Player spawn

	return 0;
}

void update()
{
	// Handle player input
	I_HandleEvents(&gameState, &entManager, &h_keyStates);
	H_HandleKeyStates(&gameState, &entManager);
	// Update AI
	G_AI_Idle(&entManager);
	G_AI_Chase(&gameState, &entManager);
	// Update physics
	P_EntityFall(&entManager, &gameState);
	P_EntityWallCollisionCheck(map, &entManager, &gameState);
	P_EntityToEntityCollisionCheck(&entManager, &gameState);
	// Update transforms
	G_SkeletonSpawn(&entManager, &spawnTimer);
	G_UpdateEntity(&gameState, &entManager);
}

void render(uint64_t* frameStart)
{
	// Clear frame
	I_FrameStart(frameStart);
	// Render
	R_RenderLocation(map, &entManager);
	R_RenderObject(&objManager, &entManager);
	R_RenderEntity(&entManager);
	R_RenderDebugStats(&gameState, &entManager);
	// Push frame
	I_FrameEnd(&gameState, frameStart);
}

void I_Loop()
{
	uint64_t frameStart = 0;
	while (gameState.isRunning)
	{
		update();
		render(&frameStart);
	}
}


