#include <stdbool.h>
#include "p_physics.h"
#include "s_system.h"
#include "typedefs.h"

float 	gravity = 2200.0f;
float 	jump_force = -800.0f;
float 	player_speed = 200.0f;
float 	camera_speed = 6.0f;
float 	knockback_strength = 300.0f;

void P_EntityFallJump(e_manager_t* pEntManager, gamestate_t* pGameState)
{
	double dt = pGameState->deltaTime;

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->isFalling[i])
			pEntManager->velocities[i].gravityAccel += gravity * dt;
		else
			pEntManager->velocities[i].gravityAccel = 0.0f;

		pEntManager->transforms[i].logY += pEntManager->velocities[i].gravityAccel * dt;

		if (pEntManager->transforms[i].logY >= FLOOR_DISTANCE)
		{
			pEntManager->transforms[i].logY = (float) FLOOR_DISTANCE;
			pEntManager->isFalling[i] = false;
		}
		else pEntManager->isFalling[i] = true;
	}
}

/*
 * This method checks entity wall collision in all of directions (left, right)
 */
void P_EntityWallCollisionCheck(location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState)
{
	const float mapWidth = (float)(pLocation->columns * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE);
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - entitySpriteSize / 2);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING) continue;

		if (pEntManager->transforms[i].logX < screenXCenter)
		{
			pEntManager->transforms[i].logX = screenXCenter;
			pEntManager->aiParams[i].isCollisionOnLeft = true;
		}
		else pEntManager->aiParams[i].isCollisionOnLeft = false;

		if (pEntManager->transforms[i].logX > mapWidth / 2 + screenXCenter / 2 + entitySpriteSize * 2)
		{
			pEntManager->transforms[i].logX = mapWidth / 2 + screenXCenter / 2 + entitySpriteSize * 2;
			pEntManager->aiParams[i].isCollisionOnRight = true;
		}
		else pEntManager->aiParams[i].isCollisionOnRight = false;
	}
}

void P_EntityToEntityCollisionCheck(e_manager_t* pEntManager, gamestate_t* pGameState)
{
	int eCount = pEntManager->entitiesCount;
	if (eCount < 2) return;

	double dt = pGameState->deltaTime;
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - entitySpriteSize / 2);

	for (int i = 0; i < eCount; ++i)
	{
		for (int j = i + 1; j < eCount; ++j)
		{
			if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING
				|| pEntManager->state[j] == STATE_SPAWNING || pEntManager->state[j] == STATE_REMOVING) continue;

			S_DestCollisionCheck(pEntManager, i, j, screenXCenter, dt);
		}
	}
}
