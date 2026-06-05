#include "g_entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "i_system.h"
#include "p_physics.h"
#include "typedefs.h"

/*
 * This function initializes entity (data-oriented style).
 */
void G_EntityInit(e_manager_t* pEntManager, int posX, int posY, float speed, float knockback, int hp, int strength, enum ENTITY_ID id)
{
	int i = pEntManager->entitiesCount;
	assert(i <= MAX_ENTITIES);
	// Texture load
	pEntManager->id[i] = id;
	pEntManager->sprites[i].srcX = 0;
	pEntManager->sprites[i].srcY = 0;
	pEntManager->sprites[i].srcW = ENTITY_SPRITE_SIZE;
	pEntManager->sprites[i].srcH = ENTITY_SPRITE_SIZE;
	// Sprite controls
	pEntManager->sprites[i].direction = RIGHT;
	pEntManager->sprites[i].currentSprite = 0;
	pEntManager->transforms[i].logX = (float) posX;
	pEntManager->transforms[i].logY = (float) posY;
	pEntManager->transforms[i].flip = 0;
	// Physics
	pEntManager->transforms[i].hitboxW = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE - 64;
	pEntManager->transforms[i].hitboxH = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	pEntManager->velocities[i].gravityAccel = 0.0;
	pEntManager->velocities[i].currentSpeed = speed;
	// Timers
	pEntManager->destructTimer[i].reactionTime = ENTITY_DESTRUCT_TIME;
	pEntManager->animTimer[i].reactionTime = ANIM_TIME;
	pEntManager->aiTimer[i].reactionTime = 0;
	// Flags
	pEntManager->isMoving[i] = false;
	pEntManager->isFalling[i] = false;
	pEntManager->state[i] = STATE_NONE;
	// AI
	pEntManager->aiParams[i].isCollisionOnLeft = false;
	pEntManager->aiParams[i].isCollisionOnRight = false;
	++pEntManager->entitiesCount;
}

void G_SkeletonSpawn(e_manager_t* pEntManager, rtimer_t* timer)
{
	int i = pEntManager->entitiesCount;

	I_ReactionTimerStart(timer);

	if (I_IsTimeToReact(timer) && pEntManager->entitiesCount < MAX_ENTITIES)
	{
		I_ReactionTimerEnd(timer);

		int randomXPos = rand() % LOGICAL_WIDTH + LOGICAL_WIDTH / 2;

		G_EntityInit(pEntManager, randomXPos, FLOOR_DISTANCE, skeleton_speed, attack_knockback, SKELETON_HP, SKELETON_STRENGTH, SKELETON);
		G_SetAi(i, pEntManager, AI_IDLE);
		G_SetState(i, pEntManager, STATE_SPAWNING);
	}
}

void G_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	double dt = pGameState->deltaTime;

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		switch (pEntManager->sprites[i].direction)
		{
		case LEFT:
			pEntManager->transforms[i].flip = 0;
			if (pEntManager->isMoving[i] && pEntManager->state[i] == STATE_NONE)
				pEntManager->transforms[i].logX -= pEntManager->velocities[i].currentSpeed * (float) dt;
			break;
		case RIGHT:
			pEntManager->transforms[i].flip = 1;
			if (pEntManager->isMoving[i] && pEntManager->state[i] == STATE_NONE)
				pEntManager->transforms[i].logX += pEntManager->velocities[i].currentSpeed * (float) dt;
			break;
		}
	}
}

void G_MarkEntityToRemove(int index, e_manager_t* pEntManager)
{
	I_ReactionTimerStart(&pEntManager->destructTimer[index]);

	if (I_IsTimeToReact(&pEntManager->destructTimer[index]))
	{
		I_ReactionTimerEnd(&pEntManager->destructTimer[index]);
		G_RemoveEntityFromLoadList(index, pEntManager);
	}
}

/*
 * This function removes the entity by its index from all of the data arrays
 * and shifts all the data to the left from removed entity's index.
 */
void G_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager)
{
	if (index <= 0 || index >= pEntManager->entitiesCount) return;

	for (int i = index; i < pEntManager->entitiesCount - 1; i++)
	{
	    pEntManager->aiParams[i] = pEntManager->aiParams[i + 1];
	    pEntManager->aiTimer[i] = pEntManager->aiTimer[i + 1];
	    pEntManager->animTimer[i] = pEntManager->animTimer[i + 1];
	    pEntManager->state[i] = pEntManager->state[i + 1];
	    pEntManager->isMoving[i] = pEntManager->isMoving[i + 1];
	    pEntManager->sprites[i] = pEntManager->sprites[i + 1];
	    pEntManager->transforms[i] = pEntManager->transforms[i + 1];
	    pEntManager->velocities[i] = pEntManager->velocities[i + 1];
	}

	--pEntManager->entitiesCount;
}

void G_AI_Idle(e_manager_t* pEntManager)
{
	for (int i = 1; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->ai[i] != AI_IDLE
		|| pEntManager->state[i] == STATE_SPAWNING
		|| pEntManager->state[i] == STATE_REMOVING) continue;

		// WHEN IN THE SPOT DISTANCE
		if (abs(pEntManager->transforms[PLAYER].logX - pEntManager->transforms[i].logX) <= SPOT_DISTANCE)
		{
			G_SetAi(i, pEntManager, AI_CHASING);
			continue;
		}

		pEntManager->isMoving[i] = true;

		if (pEntManager->aiParams[i].isCollisionOnLeft)
			pEntManager->sprites[i].direction = RIGHT;
		else if (pEntManager->aiParams[i].isCollisionOnRight)
			pEntManager->sprites[i].direction = LEFT;

		I_ReactionTimerStart(&pEntManager->aiTimer[i]);

		if (I_IsTimeToReact(&pEntManager->aiTimer[i]))
		{
			pEntManager->aiTimer[i].reactionTime = AI_IDLE_MIN_RENEW_TIME + rand() % AI_IDLE_MAX_RENEW_TIME;
			pEntManager->aiParams[i].currentChoice = rand() % 100;

			if (pEntManager->aiParams[i].currentChoice <= 50)
				pEntManager->sprites[i].direction = LEFT;
			else
				pEntManager->sprites[i].direction = RIGHT;

			I_ReactionTimerEnd(&pEntManager->aiTimer[i]);
		}
	}
}

void G_AI_Chase(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;

	for (int i = 1; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->ai[i] != AI_CHASING
		|| pEntManager->state[i] == STATE_SPAWNING
		|| pEntManager->state[i] == STATE_REMOVING) continue;

		// WHEN OUT OF SPOT DISTANCE
		if (abs(pEntManager->transforms[PLAYER].logX - pEntManager->transforms[i].logX) >= SPOT_DISTANCE)
		{
			G_SetAi(i, pEntManager, AI_IDLE);
			continue;
		}

		I_ReactionTimerStart(&pEntManager->aiTimer[i]);

		if (I_IsTimeToReact(&pEntManager->aiTimer[i]))
		{
			if (i == rand() % MAX_ENTITIES) G_SetState(i, pEntManager, STATE_ANGER);
			pEntManager->aiTimer[i].reactionTime = AI_IDLE_MIN_RENEW_TIME + rand() % AI_IDLE_MAX_RENEW_TIME;

			if (pEntManager->transforms[i].logX - entitySpriteSize > pEntManager->transforms[PLAYER].logX) // ON RIGHT
			{
				pEntManager->isMoving[i] = true;
				pEntManager->sprites[i].direction = LEFT;
			}
			else if (pEntManager->transforms[i].logX + entitySpriteSize < pEntManager->transforms[PLAYER].logX) // ON LEFT
			{
				pEntManager->isMoving[i] = true;
				pEntManager->sprites[i].direction = RIGHT;
			}
			else
			{
				pEntManager->isMoving[i] = false;
				G_SetState(i, pEntManager, STATE_ATTACK);
			}

			if (pEntManager->transforms[i].logY > pEntManager->transforms[PLAYER].logY) // ON TOP
			{
				pEntManager->isMoving[i] = true;
				G_EntityJump(pGameState, pEntManager, i);
			}

			I_ReactionTimerEnd(&pEntManager->aiTimer[i]);
		}
	}
}

void G_EntityJump(gamestate_t* pGameState, e_manager_t* pEntManager, int index)
{
	if (!pEntManager->isFalling[index])
	{
		pEntManager->velocities[index].gravityAccel = jump_force;
		pEntManager->isFalling[index] = true;
	}
}

void G_SetState(int index, e_manager_t* pEntManager, enum ENTITY_STATE state)
{
	pEntManager->sprites[index].currentSprite = 0;
	pEntManager->sprites[index].srcX = 0;
	pEntManager->sprites[index].srcY = 0;
	pEntManager->state[index] = state;
}

void G_SetAi(int index, e_manager_t* pEntManager, enum ENTITY_AI ai)
{
	pEntManager->ai[index] = ai;
}


