#ifndef G_GAMESTATE_H_
#define G_GAMESTATE_H_

#define TILE_SPRITE_SCALE 6
#define ENTITY_SPRITE_SCALE 5
#define TILE_SPRITE_SIZE 16
#define ENTITY_SPRITE_SIZE 32
#define LOGICAL_WIDTH 1920
#define LOGICAL_HEIGHT 1080

#include "typedefs.h"

/* --- DEFINITIONS --- */
gamestate_t G_GameInit();

void G_FrameStart();
void G_FrameEnd(gamestate_t* pGameState);

/* --- IMPLEMENTATIONS --- */
#if defined(STB_GAME_INIT_IMPLEMENTATION)
/*
 * This method initializes structure with main parameters and states of the game.
 */
gamestate_t G_GameInit()
{
    gamestate_t gameState = {
    	.isRunning = true,
		.isPaused = false,
		.isDebugMode = false,
		.currentFPS = 0
    };

    return gameState;
}

int frameStart = 0;

void G_FrameStart()
{
    frameStart = SDL_GetTicks();
}

void G_FrameEnd(gamestate_t* pGameState)
{
    pGameState->deltaTime = (SDL_GetTicks() - frameStart) / 1000.0;

    if (pGameState->deltaTime < pGameState->targetFrameTime)
    {
        SDL_Delay((pGameState->targetFrameTime - pGameState->deltaTime) * 1000.0);
        pGameState->deltaTime = pGameState->targetFrameTime;
    }
}
#endif /* STB_GAME_INIT_IMPLEMENTATION */

#endif /* G_GAMESTATE_H_ */
