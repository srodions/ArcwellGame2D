#include <stdlib.h>
#include <stdbool.h>
#include "h_keyboard.h"
#include "p_physics.h"
#include "e_entity.h"
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
	h_keyStates.isRemove = false;
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

	pEntManager->isMoving[0] = h_keyStates.isLeft || h_keyStates.isRight;

	if (h_keyStates.isUp || h_keyStates.isSpace)
	{
		if (!pEntManager->isFalling[0])
		{
			pEntManager->velocities[0].gravityAccel = jump_force;
			pEntManager->isFalling[0] = true;
		}
	}

	if (h_keyStates.isLeft)
		pEntManager->sprites[0].direction = LEFT;
	else if (h_keyStates.isRight)
		pEntManager->sprites[0].direction = RIGHT;

	if (h_keyStates.isDebug)
	{
		pGameState->isDebugMode = !pGameState->isDebugMode;
		h_keyStates.isDebug = false;
	}

	if (h_keyStates.isUse && pEntManager->entitiesCount < MAX_ENTITIES)
	{
		E_EntityInit(pEntManager, rand() % LOGICAL_WIDTH + 880, FLOOR_DISTANCE, player_speed, SKELETON);
		E_SetAi(pEntManager->entitiesCount - 1, pEntManager, AI_IDLE);
		E_SetState(pEntManager->entitiesCount - 1, pEntManager, STATE_SPAWNING);
		h_keyStates.isUse = false;
	}
	else if (h_keyStates.isRemove)
	{
		if (pEntManager->entitiesCount > 1)
			E_SetState(pEntManager->entitiesCount - 1, pEntManager, STATE_REMOVING);
		h_keyStates.isRemove = false;
	}
}


