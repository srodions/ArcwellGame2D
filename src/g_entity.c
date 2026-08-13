#include "g_entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "i_system.h"
#include "p_physics.h"
#include "typedefs.h"
#include "fixed_math.h"
#include "l_arcloader.h"

void G_LoadEntityConfigs(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable)
{
	uint32_t cfgEntrySize = 0;
	arcf_entcfgheader_t* entHeader = (arcf_entcfgheader_t*) L_LoadLump(arcFile, "ENTITIES", pHeader, pTable, &cfgEntrySize);

	int cfgCount = entHeader->cfgCount;
	int capacity = cfgCount * sizeof(e_config_t);

	pEntCfgManager->configs = (e_config_t*) malloc(capacity);
	pEntCfgManager->cfgCount = cfgCount;
	pEntCfgManager->capacity = capacity;

	memcpy(pEntCfgManager->configs, entHeader->items, capacity);

	for (int i = 0; i < cfgCount; ++i)
	{
		int posX = entHeader->items[i].posX;
		int posY = entHeader->items[i].posY;
		int speed = entHeader->items[i].speed;
		int maxHP = entHeader->items[i].maxHp;
		int atlasSprIdx = entHeader->items[i].atlasSprIdx;

		G_EntityInit(pEntManager, atlasSprIdx, posX, posY, speed, maxHP);
	}
}

/*
 * This function initializes entity (data-oriented style).
 *
 */
void G_EntityInit(e_manager_t* pEntManager, int atlasSprIdx, int posX, int posY, int speed, int maxHP)
{
	int i = pEntManager->entitiesCount;

	if (i >= MAX_ENTITIES) return;

	// Texture load
	pEntManager->atlasSprIdx[i] = atlasSprIdx;
	pEntManager->sprites[i].srcX = 0;
	pEntManager->sprites[i].srcY = 0;
	pEntManager->sprites[i].srcW = ENT_SPR_SIZE;
	pEntManager->sprites[i].srcH = ENT_SPR_SIZE;
	// Sprite controls
	pEntManager->sprites[i].direction = DIR_RIGHT;
	pEntManager->sprites[i].currentSprite = 0;
	pEntManager->transforms[i].logX = INT_TO_FIXED(posX);
	pEntManager->transforms[i].logY = INT_TO_FIXED(posY);
	pEntManager->transforms[i].flip = 0;
	// Physics
	pEntManager->transforms[i].hitboxW = ENT_SPR_SIZE - (ENT_SPR_SIZE / 2);
	pEntManager->transforms[i].hitboxH = ENT_SPR_SIZE - (ENT_SPR_SIZE / 2);
	pEntManager->velocities[i].gravityAccel = DOUBLE_TO_FIXED(0.0);
	pEntManager->velocities[i].currentSpeed = INT_TO_FIXED(speed);
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
	// Combat
	pEntManager->hp[i] = maxHP;
	pEntManager->hasDamaged[i] = false;

	if (i != PLAYER) 			// TODO: Maybe remove this in future
	{
		G_SetState(i, pEntManager, STATE_SPAWNING);
		G_SetAi(i, pEntManager, AI_IDLE);
	}
}

void G_EntityHPControl(gamestate_t* pGameState, e_manager_t* pEntManager, int i)
{
	if (pEntManager->hp[i] == 0 && pEntManager->state[i] != STATE_REMOVING)
	{
		if (i != PLAYER)
			G_SetState(i, pEntManager, STATE_REMOVING);
		else
			pGameState->isPaused = true;
	}
}

void G_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager)
{
	const int screenXCenter = SCR_LOGICAL_WIDTH / 2 - ENT_SPR_SIZE / 2;
	const fixed_t screenXCenterFixed = INT_TO_FIXED(screenXCenter);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		int screenX = i > 0 ? FIXED_TO_INT(pEntManager->transforms[i].logX - pEntManager->transforms[PLAYER].logX + screenXCenterFixed) : screenXCenter;
		int screenY = FIXED_TO_INT(pEntManager->transforms[i].logY);

		// Entity culling
		if (screenX + ENT_SPR_SIZE < 0
			|| screenX > SCR_LOGICAL_WIDTH
			|| screenY < 0
			|| screenY > SCR_LOGICAL_HEIGHT)
		{
			continue;
		}

		G_EntityDirection(pGameState, pEntManager, i);
		G_AI_Idle(pEntManager, i);
		G_AI_Chase(pGameState, pEntManager, pEntCfgManager, i);
		G_EntityAttack(pEntManager, pEntCfgManager, i, PLAYER); // Entity attacks
		G_EntityAttack(pEntManager, pEntCfgManager, PLAYER, i); // Player attacks
		G_EntityHPControl(pGameState, pEntManager, i);
	}
}

void G_EntityDirection(gamestate_t* pGameState, e_manager_t* pEntManager, int i)
{
	fixed_t dt = pGameState->deltaTime;
	fixed_t speed = FIX_MUL(pEntManager->velocities[i].currentSpeed, dt);

	switch (pEntManager->sprites[i].direction)
	{
	case DIR_LEFT:
		pEntManager->transforms[i].flip = 0;
		if (pEntManager->isMoving[i] && pEntManager->state[i] == STATE_NONE)
			pEntManager->transforms[i].logX -= speed;
		break;
	case DIR_RIGHT:
		pEntManager->transforms[i].flip = 1;
		if (pEntManager->isMoving[i] && pEntManager->state[i] == STATE_NONE)
			pEntManager->transforms[i].logX += speed;
		break;
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
	    pEntManager->hp[i] = pEntManager->hp[i + 1];
	    pEntManager->hasDamaged[i] = pEntManager->hasDamaged[i + 1];
	}

	--pEntManager->entitiesCount;
}

void G_AI_Idle(e_manager_t* pEntManager, int i)
{
	if (pEntManager->ai[i] != AI_IDLE
	|| pEntManager->state[i] == STATE_SPAWNING
	|| pEntManager->state[i] == STATE_REMOVING) return;

	// WHEN IN THE SPOT DISTANCE
	if (abs(FIXED_TO_INT(pEntManager->transforms[PLAYER].logX) - FIXED_TO_INT(pEntManager->transforms[i].logX)) <= SPOT_DISTANCE)
	{
		G_SetAi(i, pEntManager, AI_CHASING);
		return;
	}

	pEntManager->isMoving[i] = true;

	if (pEntManager->aiParams[i].isCollisionOnLeft)
		pEntManager->sprites[i].direction = DIR_RIGHT;
	else if (pEntManager->aiParams[i].isCollisionOnRight)
		pEntManager->sprites[i].direction = DIR_LEFT;

	I_ReactionTimerStart(&pEntManager->aiTimer[i]);

	if (I_IsTimeToReact(&pEntManager->aiTimer[i]))
	{
		pEntManager->aiTimer[i].reactionTime = AI_IDLE_MIN_RENEW_TIME + rand() % AI_IDLE_MAX_RENEW_TIME;
		pEntManager->aiParams[i].currentChoice = rand() % 100;

		if (pEntManager->aiParams[i].currentChoice <= 50)
			pEntManager->sprites[i].direction = DIR_LEFT;
		else
			pEntManager->sprites[i].direction = DIR_RIGHT;

		I_ReactionTimerEnd(&pEntManager->aiTimer[i]);
	}
}

void G_AI_Chase(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	if (pEntManager->ai[i] != AI_CHASING
	|| pEntManager->state[i] == STATE_SPAWNING
	|| pEntManager->state[i] == STATE_REMOVING) return;

	// WHEN OUT OF SPOT DISTANCE
	if (abs(FIXED_TO_INT(pEntManager->transforms[PLAYER].logX) - FIXED_TO_INT(pEntManager->transforms[i].logX)) >= SPOT_DISTANCE)
	{
		G_SetAi(i, pEntManager, AI_IDLE);
		return;
	}

	int chaserAttackDist = pEntCfgManager->configs[pEntManager->atlasSprIdx[i]].attackDist;
	fixed_t stopDist = INT_TO_FIXED(chaserAttackDist);

	I_ReactionTimerStart(&pEntManager->aiTimer[i]);

	if (I_IsTimeToReact(&pEntManager->aiTimer[i]))
	{
		if (i == rand() % MAX_ENTITIES)
		{
			if (pEntManager->state[i] != STATE_ANGER) G_SetState(i, pEntManager, STATE_ANGER);
		}
		pEntManager->aiTimer[i].reactionTime = AI_IDLE_MIN_RENEW_TIME + rand() % AI_IDLE_MAX_RENEW_TIME;

		if (pEntManager->transforms[i].logX - stopDist > pEntManager->transforms[PLAYER].logX) // ON RIGHT
		{
			pEntManager->isMoving[i] = true;
			pEntManager->sprites[i].direction = DIR_LEFT;
		}
		else if (pEntManager->transforms[i].logX + stopDist < pEntManager->transforms[PLAYER].logX) // ON LEFT
		{
			pEntManager->isMoving[i] = true;
			pEntManager->sprites[i].direction = DIR_RIGHT;
		}
		else
		{
			pEntManager->isMoving[i] = false;
			if (pEntManager->state[i] != STATE_ATTACK) G_SetState(i, pEntManager, STATE_ATTACK);
		}

		if (pEntManager->transforms[i].logY > pEntManager->transforms[PLAYER].logY) // ON TOP
		{
			pEntManager->isMoving[i] = true;
			G_EntityJump(pGameState, pEntManager, i);
		}

		I_ReactionTimerEnd(&pEntManager->aiTimer[i]);
	}
}

void G_EntityAttack(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int attackerId, int victimId)
{
	if (pEntManager->state[attackerId] != STATE_ATTACK || (victimId == PLAYER && attackerId == PLAYER)
		|| pEntManager->state[victimId] == STATE_REMOVING || pEntManager->hp[victimId] == 0) return;

	int deltaX = FIXED_TO_INT(pEntManager->transforms[victimId].logX - pEntManager->transforms[attackerId].logX);
	int deltaY = FIXED_TO_INT(pEntManager->transforms[victimId].logY - pEntManager->transforms[attackerId].logY);

	int distance = abs(deltaX) + abs(deltaY);

	e_config_t attackerCfg = pEntCfgManager->configs[pEntManager->atlasSprIdx[attackerId]];

	if (distance <= attackerCfg.attackDist
		&& !pEntManager->hasDamaged[attackerId]
		&& pEntManager->sprites[attackerId].currentSprite >= attackerCfg.dmgSpriteIndex)
	{
		pEntManager->hp[victimId] -= attackerCfg.strength;
		pEntManager->hasDamaged[attackerId] = true;

		int directionX = (deltaX > 0) ? 1 : -1;
		int directionY = (deltaY > 0) ? 1 : -1;
		pEntManager->transforms[victimId].logX += INT_TO_FIXED(directionX * TILE_SPR_SIZE / 2);
		pEntManager->transforms[victimId].logY += INT_TO_FIXED(directionY * TILE_SPR_SIZE / 2);
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


