#include <stdbool.h>
#include "g_gamestate.h"
#include "typedefs.h"

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
