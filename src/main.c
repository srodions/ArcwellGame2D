#define SDL_MAIN_HANDLED

#define STB_GAME_INIT_IMPLEMENTATION
#define STB_KEYBOARD_HANDLER_IMPLEMENTATION
#define STB_EVENT_HANDLER_IMPLEMENTATION
#define STB_ENTITY_IMPLEMENTATION
#define STB_ENTITY_AI_IMPLEMENTATION
#define STB_RENDERER_IMPLEMENTATION
#define STB_LOCATION_IMPLEMENTATION
#define STB_PHYSICS_IMPLEMENTATION
#define STB_REACTION_TIMER_IMPLEMENTATION

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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

void loop(SDL_Renderer* pRenderer, gamestate_t* pGameState, location_t* pCurrentLocation, e_manager_t* pEntManager)
{
	while (pGameState->isRunning)
	{
		G_FrameStart();
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		R_RenderLocation(pRenderer, pCurrentLocation);

		// Player controls
		K_HandleEvents(pRenderer, pGameState, pEntManager);

		// Entities
		E_AI_Idle(pEntManager);
		P_EntityFallJump(pEntManager, pGameState);
		E_EntityWallCollisionCheck(pCurrentLocation, pEntManager, pGameState);
		E_EntityToEntityCollisionCheck(pEntManager, pGameState);

		R_RenderEntity(pRenderer, pCurrentLocation, pEntManager, pGameState);
		R_RenderStats(pRenderer, pGameState, &pEntManager->entitiesCount);

		SDL_RenderPresent(pRenderer);
		G_FrameEnd(pGameState);
	}
}

int main()
{
	gamestate_t gameState = G_GameInit();
	e_manager_t entityManager = { .entitiesCount = 0 };

	K_InitKeymap();
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_Window* pWindow = R_WindowInit(&gameState);
	SDL_Renderer* pRenderer = R_RendererInit(pWindow);
	if (pRenderer == NULL) return 1;

	E_SpritesInit(pRenderer, &entityManager);

	srand(time(NULL));

	location_t currentLocation = L_LocationInit(pRenderer, "res/location/tomb.dat");
	E_EntityInit(pRenderer, &entityManager, 0, 50, FLOOR_DISTANCE);

	loop(pRenderer, &gameState, &currentLocation, &entityManager);

	TTF_CloseFont(font.file);
	SDL_DestroyWindow(pWindow);
	SDL_DestroyRenderer(pRenderer);
	SDL_Quit();

	return 0;
}


