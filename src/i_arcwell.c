#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "g_entity.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "g_gamestate.h"
#include "g_map.h"
#include "h_input.h"
#include "i_system.h"
#include "r_renderer.h"
#include "typedefs.h"

gamestate_t gameState;
map_t* map;
obj_manager_t objManager;
e_manager_t entManager;
rtimer_t spawnTimer;
inputstate_t input;

int I_GameInit()
{
	if (I_LibInit() < 0) return -1;

	I_InitKeymap();
	I_InitBtnMap();
	input.current = 0;
	input.previous = 0;
	gameState = G_GameInit();

	if (I_WindowInit(&gameState) < 0 || I_RendererInit() < 0) return -1;

	entManager.entitiesCount = 0;
	spawnTimer.reactionTime = ENTITY_SPAWN_TIME;
	objManager.objCount = 0;

	FILE* arcFile = fopen("assets.arc", "rb");
	if (!arcFile) return -1;
	arcf_header_t* header = L_LoadHeader(arcFile);
	if (!header) return -1;
	arcf_entry_t* table = L_LoadLumpsTable(arcFile, header);
	if (!table) return -1;
	I_LoadFont(arcFile, header, table, 16);
	R_LoadSpritesData(arcFile, header, table);

	map = G_MapInit(arcFile, header, table, &objManager);
	G_ObjSetter(arcFile, header, table, &objManager);
	fclose(arcFile);
	free(header);
	free(table);

	G_CreatePlayerConfig();
	G_CreateSkeletonConfig();
	G_EntityInit(&entManager, PLAYER, 50, FLOOR_DISTANCE, &configs[PLAYER]); // Player spawn

	return 0;
}

void update()
{
	// Handle window events
	I_HandleEvents(&gameState, &entManager, &input);
	// Handle player input
	H_HandleKeyStates(&gameState, &entManager, &input);

	if (!gameState.isPaused)
	{
		// Update physics
		P_EntityFall(&entManager, &gameState);
		P_EntityWallCollisionCheck(map, &entManager, &gameState);
		P_EntityToEntityCollisionCheck(&entManager, &gameState);
		// Update transforms/AI
		G_SkeletonSpawn(&entManager, &spawnTimer);
		G_UpdateEntity(&gameState, &entManager);
	}
}

void render(uint64_t* frameStart)
{
	// Clear frame
	I_FrameStart(frameStart);
	// Render
	I_RenderScene(map, &objManager, &entManager);
	//R_RenderDebugStats(&gameState, &entManager);
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


