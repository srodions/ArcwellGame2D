#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "s_system.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "e_entity.h"
#include "g_gamestate.h"
#include "h_keyboard.h"
#include "r_renderer.h"
#include "l_location.h"
#include "typedefs.h"

gamestate_t gameState;
location_t* location;
obj_manager_t objManager;
e_manager_t entManager;

int init()
{
	if (S_LibInit() < 0) return -1;

	S_InitKeymap();
	gameState = G_GameInit();

	if (S_WindowInit(&gameState) < 0 || S_RendererInit() < 0) return -1;

	S_FontInit();
	entManager.entitiesCount = 0;
	objManager.objCount = 0;

	FILE* arcFile = fopen("res/assets.arc", "rb");
	if (!arcFile) return -1;
	arcf_header_t* header = L_LoadHeader(arcFile);
	if (!header) return -1;
	arcf_entry_t* table = L_LoadLumpsTable(arcFile, header);
	if (!table) return -1;
	S_ARC_InitTextures(arcFile, header, table);
	location = L_LocationInit(arcFile, header, table, &objManager);
	S_ObjectSetter(&objManager, "res/location/objects.json");
	fclose(arcFile);
	free(header);
	free(table);

	E_EntityInit(&entManager, 1050, FLOOR_DISTANCE, player_speed, PLAYER); // Player spawn

	return 0;
}

void update()
{
	// Handle player input
	S_HandleEvents(&gameState, &entManager, &h_keyStates);
	H_HandleKeyStates(&gameState, &entManager);
	// Update AI
	E_AI_Idle(&entManager);
	// Update physics
	P_EntityFallJump(&entManager, &gameState);
	P_EntityWallCollisionCheck(location, &entManager, &gameState);
	P_EntityToEntityCollisionCheck(&entManager, &gameState);
	// Update transforms
	E_UpdateEntity(&gameState, &entManager);
}

void render(int* frameStart)
{
	// Clear frame
	S_FrameStart(frameStart);
	// Render
	R_RenderLocation(location, &entManager);
	R_RenderObject(&objManager, &entManager);
	R_RenderEntity(&entManager);
	R_RenderDebugStats(&gameState, &entManager);
	// Push frame
	S_FrameEnd(&gameState, frameStart);
}

void loop()
{
	int frameStart = 0;
	while (gameState.isRunning)
	{
		update();
		render(&frameStart);
	}
}

int main(int argc, char* argv[])
{
	if (init() < 0)
	{
		S_Destruct(&objManager, &entManager);
		return -1;
	}

	loop();
	S_Destruct(&objManager, &entManager);

	return 0;
}


