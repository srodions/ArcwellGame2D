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
obj_manager_t objManager;
e_manager_t entManager;
e_cfgmanager_t entCfgManager;
map_manager_t mapManager;
rtimer_t spawnTimer;
inputstate_t input;
// Tables
arcf_namesentry_t* sprNamesTable;

int I_GameInit()
{
	if (I_LibInit() < 0) return -1;

	if (I_WindowInit(&gameState) < 0 || I_RendererInit() < 0) return -1;

	I_InitKeymap();
	I_InitBtnMap();

	input.current = 0;
	input.previous = 0;
	gameState = G_GameStateInit();

	entManager.entitiesCount = 0;
	objManager.objCount = 0;
	mapManager.mapsCount = 0;
	spawnTimer.reactionTime = ENTITY_SPAWN_TIME;

	FILE* arcFile = fopen("assets.arc", "rb");
	if (!arcFile) return -1;
	arcf_header_t* header = L_LoadHeader(arcFile);
	if (!header) return -1;
	arcf_entry_t* table = L_LoadLumpsTable(arcFile, header);
	if (!table) return -1;

	// Loads sprites names table
	uint32_t currentDataSize = 0;
	sprNamesTable = (arcf_namesentry_t*) L_LoadLump(arcFile, "SPRNAMES", header, table, &currentDataSize);
	R_LoadSpritesData(arcFile, header, table, sprNamesTable);

	mapManager.maps = (map_t*) malloc(sizeof(map_t));
	G_MapSetter(&mapManager, arcFile, header, table, "TOMB");
	G_ObjSetter(arcFile, header, table, &objManager);
	G_LoadEntityConfigs(&entManager, &entCfgManager, arcFile, header, table);

	fclose(arcFile);
	free(header);
	free(table);

	return 0;
}

void update()
{
	// Handle window events
	I_HandleEvents(&gameState, &entManager, &input);
	// Handle player input
	H_HandleKeyStates(&gameState, &entManager, &entCfgManager, &input);

	if (!gameState.isPaused)
	{
		// Update physics
		P_EntityFall(&entManager, &gameState);
		P_EntityWallCollisionCheck(&mapManager, 0, &entManager, &gameState);
		P_EntityToEntityCollisionCheck(&entManager, &gameState);
		// Update transforms/AI
		G_UpdateEntity(&gameState, &entManager, &entCfgManager);
	}

	G_UpdateDebugStats(&gameState, &entManager);
}

void render(uint64_t* frameStart)
{
	// Clear frame
	I_FrameStart(frameStart);
	// Push all the objects to the screen buffer
	R_PushScene(&gameState, &mapManager, &objManager, &entManager, &entCfgManager);
	// Render frame
	I_FrameEnd(&gameState, frameStart);
}

void I_GameLoop()
{
	uint64_t frameStart = 0;
	while (gameState.isRunning)
	{
		update();
		render(&frameStart);
	}
}

void I_GameExit()
{
	R_Destruct(sprNamesTable);
	I_SDL_Destruct();
}


