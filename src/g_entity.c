#include "g_entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "i_system.h"
#include "p_physics.h"
#include "typedefs.h"
#include "fixed_math.h"

e_config_t configs[MAX_CONFIGS];

void G_CreatePlayerConfig()
{
	e_config_t config = {
		.spawnFramesCount = 0,
		.spawnFramesRow = 0,
		.deathFramesCount = 0,
		.deathFramesRow = 0,
		.angerFramesCount = 0,
		.angerFramesRow = 0,
		.attackFramesCount = 0,
		.attackFramesRow = 0,
		.maxHp = PLAYER_HP,
		.strength = PLAYER_STRENGTH,
		.knockback = knockback_strength,
		.attackDist = PLAYER_ATK_DIST,
		.dmgSpriteIndex = PLAYER_DMG_SPR,
		.walkFramesCount = WALK_FRAMES_COUNT,
		.walkFramesRow = 0,
		.speed = player_speed
	};

	configs[PLAYER] = config;
}

void G_CreateSkeletonConfig()
{
	e_config_t config = {
		.spawnFramesCount = SPAWN_FRAMES_COUNT,
		.spawnFramesRow = 1,
		.deathFramesCount = DEATH_FRAMES_COUNT,
		.deathFramesRow = 2,
		.angerFramesCount = ANGER_FRAMES_COUNT,
		.angerFramesRow = 3,
		.attackFramesCount = ATTACK_FRAMES_COUNT,
		.attackFramesRow = 4,
		.maxHp = SKELETON_HP,
		.strength = SKELETON_STRENGTH,
		.knockback = knockback_strength,
		.attackDist = SKELETON_ATK_DIST,
		.dmgSpriteIndex = SKELETON_DMG_SPR,
		.walkFramesCount = WALK_FRAMES_COUNT,
		.walkFramesRow = 0,
		.speed = skeleton_speed
	};

	configs[SKELETON] = config;
}

/*
 * This function initializes entity (data-oriented style).
 *
 */
void G_EntityInit(e_manager_t* pEntManager, enum ENTITY_ID id, int posX, int posY, e_config_t* config)
{
	int i = pEntManager->entitiesCount;

	if (i >= MAX_ENTITIES) return;

	// Texture load
	pEntManager->id[i] = id;
	pEntManager->sprites[i].srcX = 0;
	pEntManager->sprites[i].srcY = 0;
	pEntManager->sprites[i].srcW = ENTITY_SPRITE_SIZE;
	pEntManager->sprites[i].srcH = ENTITY_SPRITE_SIZE;
	// Sprite controls
	pEntManager->sprites[i].direction = DIR_RIGHT;
	pEntManager->sprites[i].currentSprite = 0;
	pEntManager->transforms[i].logX = INT_TO_FIXED(posX);
	pEntManager->transforms[i].logY = INT_TO_FIXED(posY);
	pEntManager->transforms[i].flip = 0;
	// Physics
	pEntManager->transforms[i].hitboxW = (ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE) - (ENTITY_SPRITE_SIZE / 2);
	pEntManager->transforms[i].hitboxH = (ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE) - (ENTITY_SPRITE_SIZE / 2);
	pEntManager->velocities[i].gravityAccel = DOUBLE_TO_FIXED(0.0);
	pEntManager->velocities[i].currentSpeed = config->speed;
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
	pEntManager->hp[i] = config->maxHp;
	pEntManager->hasDamaged[i] = false;
}

void G_SkeletonSpawn(e_manager_t* pEntManager, rtimer_t* timer)
{
	int i = pEntManager->entitiesCount;

	I_ReactionTimerStart(timer);

	if (I_IsTimeToReact(timer) && pEntManager->entitiesCount < MAX_ENTITIES)
	{
		I_ReactionTimerEnd(timer);

		int randomXPos = rand() % LOGICAL_WIDTH + LOGICAL_WIDTH / 2;

		G_EntityInit(pEntManager, SKELETON, randomXPos, FLOOR_DISTANCE, &configs[SKELETON]);
		G_SetAi(i, pEntManager, AI_IDLE);
		G_SetState(i, pEntManager, STATE_SPAWNING);
	}
}

void G_EntityHPControl(gamestate_t* pGameState, e_manager_t* pEntManager, int i)
{
	if (pEntManager->hp[i] == 0)
	{
		if (i != PLAYER)
			pEntManager->state[i] = STATE_REMOVING;
		else
			pGameState->isPaused = true;
	}
}

void G_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	const int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const int screenXCenter = LOGICAL_WIDTH / 2 - entitySpriteSize / 2;
	const fixed_t screenXCenterFixed = INT_TO_FIXED(screenXCenter);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		int screenX = i > 0 ? FIXED_TO_INT(pEntManager->transforms[i].logX - pEntManager->transforms[PLAYER].logX + screenXCenterFixed) : screenXCenter;
		int screenY = FIXED_TO_INT(pEntManager->transforms[i].logY);

		// Entity culling
		if (screenX + entitySpriteSize < 0
			|| screenX > LOGICAL_WIDTH
			|| screenY < 0
			|| screenY > LOGICAL_HEIGHT)
		{
			continue;
		}

		G_EntityDirection(pGameState, pEntManager, i);
		G_AI_Idle(pEntManager, i);
		G_AI_Chase(pGameState, pEntManager, i);
		G_EntityAttack(pEntManager, i, PLAYER); // Entity attacks
		G_EntityAttack(pEntManager, PLAYER, i); // Player attacks
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

void G_AI_Chase(gamestate_t* pGameState, e_manager_t* pEntManager, int i)
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

	fixed_t stopDist = INT_TO_FIXED(configs[pEntManager->id[i]].attackDist);

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

void G_EntityAttack(e_manager_t* pEntManager, int attackerId, int victimId)
{
	if (pEntManager->state[attackerId] != STATE_ATTACK || (victimId == PLAYER && attackerId == PLAYER)
		|| pEntManager->state[victimId] == STATE_REMOVING) return;

	int deltaX = FIXED_TO_INT(pEntManager->transforms[victimId].logX - pEntManager->transforms[attackerId].logX);
	int deltaY = FIXED_TO_INT(pEntManager->transforms[victimId].logY - pEntManager->transforms[attackerId].logY);
	int tileSize = TILE_SPRITE_SCALE * TILE_SPRITE_SIZE;

	int distance = abs(deltaX) + abs(deltaY);

	if (distance <= configs[pEntManager->id[attackerId]].attackDist
		&& !pEntManager->hasDamaged[attackerId]
		&& pEntManager->sprites[attackerId].currentSprite >= configs[pEntManager->id[attackerId]].dmgSpriteIndex)
	{
		pEntManager->hp[victimId] -= configs[pEntManager->id[attackerId]].strength;
		pEntManager->hasDamaged[attackerId] = true;

		int directionX = (deltaX > 0) ? 1 : -1;
		int directionY = (deltaY > 0) ? 1 : -1;
		pEntManager->transforms[victimId].logX += INT_TO_FIXED(directionX * tileSize / 2);
		pEntManager->transforms[victimId].logY += INT_TO_FIXED(directionY * tileSize / 2);
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


