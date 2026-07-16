#include <stdlib.h>
#include <stdbool.h>
#include "h_keyboard.h"

#include "g_entity.h"
#include "p_physics.h"
#include "typedefs.h"

keystates_t h_keyStates;

void H_InitKeyStates()
{
	h_keyStates.isUp = false;
	h_keyStates.isDown = false;
	h_keyStates.isLeft = false;
	h_keyStates.isRight = false;
	h_keyStates.isSpace = false;
	h_keyStates.isUse = false;
	h_keyStates.isDebug = false;
	h_keyStates.isCancel = false;
	h_keyStates.isExit = false;
}

/*
 * This function process keys' states boolean (true - key is pressed, false - key is released).
 * If key pressed is true then do a thing, if false - stop it.
 */
void H_HandleKeyStates(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (h_keyStates.isExit)
		pGameState->isRunning = false;

	if (h_keyStates.isDebug)
	{
		pGameState->isDebugMode = !pGameState->isDebugMode;
		h_keyStates.isDebug = false;
	}

	if (!pGameState->isPaused)
	{
		pEntManager->isMoving[PLAYER] = h_keyStates.isLeft || h_keyStates.isRight;

		if (h_keyStates.isSpace)
			G_EntityJump(pGameState, pEntManager, PLAYER);

		if (h_keyStates.isLeft)
			pEntManager->sprites[PLAYER].direction = LEFT;
		else if (h_keyStates.isRight)
			pEntManager->sprites[PLAYER].direction = RIGHT;

		if (h_keyStates.isUse)
			pEntManager->state[PLAYER] = STATE_ATTACK;
	}
}


