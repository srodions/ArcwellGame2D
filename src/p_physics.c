#include <stdbool.h>
#include "p_physics.h"
#include "s_system.h"
#include "typedefs.h"

float 	gravity = 2200.0f;
float 	jump_force = -1000.0f;
float	skeleton_speed = 125.0f;
float 	player_speed = 180.0f;
float 	camera_speed = 6.0f;
float 	knockback_strength = 300.0f;

void P_EntityFall(e_manager_t* pEntManager, gamestate_t* pGameState)
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
void P_EntityWallCollisionCheck(map_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState)
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

bool P_IntersectRect(const rect_t* a, const rect_t* b, rect_t* result)
{
    if (!a || !b || !result) return false;

    float minX = (a->x > b->x) ? a->x : b->x;
    float maxX = ((a->x + a->w) < (b->x + b->w)) ? (a->x + a->w) : (b->x + b->w);
    float minY = (a->y > b->y) ? a->y : b->y;
    float maxY = ((a->y + a->h) < (b->y + b->h)) ? (a->y + a->h) : (b->y + b->h);

    if (maxX > minX && maxY > minY)
    {
        result->x = minX;
        result->y = minY;
        result->w = maxX - minX;
        result->h = maxY - minY;
        return true;
    }

    return false;
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
            if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING ||
                pEntManager->state[j] == STATE_SPAWNING || pEntManager->state[j] == STATE_REMOVING) continue;

            rect_t a, b, result;

            a.x = pEntManager->transforms[i].logX + screenXCenter;
            a.y = pEntManager->transforms[i].logY;
            a.w = pEntManager->transforms[i].hitboxW;
            a.h = pEntManager->transforms[i].hitboxH;

            b.x = pEntManager->transforms[j].logX + screenXCenter;
            b.y = pEntManager->transforms[j].logY;
            b.w = pEntManager->transforms[j].hitboxW;
            b.h = pEntManager->transforms[j].hitboxH;

            if (P_IntersectRect(&a, &b, &result))
            {
                if (result.w >= result.h)
                {
                    if (a.y + a.h / 2.0f < b.y + b.h / 2.0f) // Top edge collision
                    {
                        pEntManager->transforms[i].logY -= knockback_strength * dt;
                        pEntManager->transforms[j].logY += knockback_strength * dt;
                    }
                    else                                     // Bottom edge collision
                    {
                        pEntManager->transforms[i].logY += knockback_strength * dt;
                        pEntManager->transforms[j].logY -= knockback_strength * dt;
                    }
                }
                else
                {
                    if (a.x + a.w / 2.0f < b.x + b.w / 2.0f) // Left edge collision
                    {
                        pEntManager->transforms[i].logX -= knockback_strength * dt;
                        pEntManager->transforms[j].logX += knockback_strength * dt;
                    }
                    else                                     // Right edge collision
                    {
                        pEntManager->transforms[i].logX += knockback_strength * dt;
                        pEntManager->transforms[j].logX -= knockback_strength * dt;
                    }
                }
            }
        }
    }
}
