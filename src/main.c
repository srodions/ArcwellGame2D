#define SDL_MAIN_HANDLED

#define STB_GAME_INIT_IMPLEMENTATION
#define STB_KEYBOARD_HANDLER_IMPLEMENTATION
#define STB_EVENT_HANDLER_IMPLEMENTATION
#define STB_ENTITY_IMPLEMENTATION
#define STB_ENTITY_AI_IMPLEMENTATION
#define STB_RENDERER_IMPLEMENTATION
#define STB_LOCATION_IMPLEMENTATION
#define STB_PHYSICS_IMPLEMENTATION
#define STB_UTILITY_IMPLEMENTATION

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "g_gamestate.h"
#include "k_keyboard.h"
#include "r_renderer.h"
#include "l_location.h"
#include "e_entity.h"
#include "p_physics.h"

SDL_Window* pWindow;
SDL_Renderer* pRenderer;
gamestate_t gameState;
location_t location;
obj_manager_t objManager;
e_manager_t entManager;

void init()
{
	K_InitKeymap();
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	srand(time(NULL));

	gameState = G_GameInit();
	pWindow = R_WindowInit(&gameState);
	pRenderer = R_RendererInit(pWindow);
	if (pRenderer == NULL || pWindow == NULL) return;

	objManager.objCount = 0;
	entManager.entitiesCount = 0;
	location = L_LocationInit(pRenderer, &objManager, "res/location/tomb.dat");
	L_ObjectSpritesInit(pRenderer);
	L_ObjectSetter(&objManager);
	E_EntitySpritesInit(pRenderer);
	E_EntityInit(&entManager, 0, 50, FLOOR_DISTANCE); // Player spawn
}

void loop()
{
	while (gameState.isRunning)
	{
		G_FrameStart();
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		R_RenderLocation(pRenderer, &location);
		R_RenderObject(pRenderer, &location, &objManager);

		// Player controls
		K_HandleEvents(pRenderer, &gameState, &entManager);

		// Entities
		E_AI_Idle(&entManager);
		P_EntityFallJump(&entManager, &gameState);
		E_EntityWallCollisionCheck(&location, &entManager, &gameState);
		E_EntityToEntityCollisionCheck(&entManager, &gameState);

		R_RenderEntity(pRenderer, &location, &entManager, &gameState);
		R_RenderStats(pRenderer, &gameState, &entManager.entitiesCount);

		SDL_RenderPresent(pRenderer);
		G_FrameEnd(&gameState);
	}
}

int main()
{
	init();

	if (pRenderer == NULL || pWindow == NULL)
		destruct(pWindow, pRenderer, &font, &entManager, &objManager, &location);

	loop();
	destruct(pWindow, pRenderer, &font, &entManager, &objManager, &location);

	return 0;
}


