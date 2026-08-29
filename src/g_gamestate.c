#include <stdbool.h>
#include <stdio.h>
#include "g_gamestate.h"
#include "g_entity.h"
#include "typedefs.h"

/*
 * This method initializes structure with main parameters and states of the game.
 */
gamestate_t G_GameStateInit()
{
    gamestate_t gameState = {
    	.isRunning = true,
		.isPaused = false,
		.isDebugMode = false,
		.isPlayerDead = false,
		.isGamepadAttached = false,
		.currentFPS = 0
    };

    return gameState;
}

void G_UpdateDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (!pGameState->isDebugMode) return;

	if (pGameState->deltaTime > 0)
		pGameState->currentFPS = (int)(FIXED_ONE / pGameState->deltaTime);

	snprintf(
		pGameState->debugText, sizeof(pGameState->debugText),
		"FPS:%d Entities:%d x:%d y:%d HP:%d",
		pGameState->currentFPS, pEntManager->entitiesCount,
		FIXED_TO_INT(pEntManager->transforms[PLAYER].logX),
		FIXED_TO_INT(pEntManager->transforms[PLAYER].logY),
		pEntManager->hp[PLAYER]
	);
}

void G_GameRestart(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager)
{
	if (!pGameState->isPlayerDead) return;

	G_EntityRespawn(pGameState, pEntManager, pEntCfgManager);

	pGameState->isPaused = false;
	pGameState->isPlayerDead = false;
}
