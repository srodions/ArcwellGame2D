#include <stdbool.h>
#include "p_physics.h"

#include "i_system.h"
#include "fixed_math.h"
#include "typedefs.h"

fixed_t 	gravity 			= DOUBLE_TO_FIXED(GRAVITY);
fixed_t 	jump_force 			= DOUBLE_TO_FIXED(JUMP_FORCE);
fixed_t		skeleton_speed 		= DOUBLE_TO_FIXED(SKELETON_SPEED);
fixed_t 	player_speed 		= DOUBLE_TO_FIXED(PLAYER_SPEED);
fixed_t 	knockback_strength 	= DOUBLE_TO_FIXED(KNOCKBACK_STRENGTH);

void P_EntityFall(e_manager_t* pEntManager, gamestate_t* pGameState)
{
	fixed_t dt = pGameState->deltaTime;
	fixed_t floorDist = INT_TO_FIXED(FLOOR_DISTANCE);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->isFalling[i])
			pEntManager->velocities[i].gravityAccel += FIX_MUL(gravity, dt);
		else
			pEntManager->velocities[i].gravityAccel = DOUBLE_TO_FIXED(0.0);

		pEntManager->transforms[i].logY += FIX_MUL(pEntManager->velocities[i].gravityAccel, dt);

		if (pEntManager->transforms[i].logY >= floorDist)
		{
			pEntManager->transforms[i].logY = floorDist;
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
	const int mapWidth = pLocation->columns * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	const int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;

	const int screenXCenter = LOGICAL_WIDTH / 2 - entitySpriteSize / 2;
	const fixed_t screenXCenterFixed = INT_TO_FIXED(screenXCenter);
	const int screenXEnd = mapWidth / 2 + screenXCenter / 2 + entitySpriteSize * 2;
	const fixed_t screenXEndFixed = INT_TO_FIXED(screenXEnd);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING) continue;

		if (pEntManager->transforms[i].logX < screenXCenterFixed)
		{
			pEntManager->transforms[i].logX = screenXCenterFixed;
			pEntManager->aiParams[i].isCollisionOnLeft = true;
		}
		else pEntManager->aiParams[i].isCollisionOnLeft = false;

		if (pEntManager->transforms[i].logX > screenXEndFixed)
		{
			pEntManager->transforms[i].logX = screenXEndFixed;
			pEntManager->aiParams[i].isCollisionOnRight = true;
		}
		else pEntManager->aiParams[i].isCollisionOnRight = false;
	}
}

bool P_IntersectRect(const rect_t* a, const rect_t* b, rect_t* result)
{
    if (!a || !b || !result) return false;

    int minX = (a->x > b->x) ? a->x : b->x;
    int maxX = ((a->x + a->w) < (b->x + b->w)) ? (a->x + a->w) : (b->x + b->w);
    int minY = (a->y > b->y) ? a->y : b->y;
    int maxY = ((a->y + a->h) < (b->y + b->h)) ? (a->y + a->h) : (b->y + b->h);

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

    fixed_t dt = pGameState->deltaTime;
    int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
    const int screenXCenter = LOGICAL_WIDTH / 2 - entitySpriteSize / 2;

    for (int i = 0; i < eCount; ++i)
    {
        for (int j = i + 1; j < eCount; ++j)
        {
            if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING ||
                pEntManager->state[j] == STATE_SPAWNING || pEntManager->state[j] == STATE_REMOVING) continue;

            rect_t a, b, result;

            a.x = FIXED_TO_INT(pEntManager->transforms[i].logX) + screenXCenter;
            a.y = FIXED_TO_INT(pEntManager->transforms[i].logY);
            a.w = pEntManager->transforms[i].hitboxW;
            a.h = pEntManager->transforms[i].hitboxH;

            b.x = FIXED_TO_INT(pEntManager->transforms[j].logX) + screenXCenter;
            b.y = FIXED_TO_INT(pEntManager->transforms[j].logY);
            b.w = pEntManager->transforms[j].hitboxW;
            b.h = pEntManager->transforms[j].hitboxH;

            if (P_IntersectRect(&a, &b, &result))
            {
				fixed_t knockbackStep = FIX_MUL(knockback_strength, dt);

                if (result.w >= result.h)
                {

                    if (a.y + a.h / 2 < b.y + b.h / 2) // Top edge collision
                    {
                        pEntManager->transforms[i].logY -= knockbackStep;
                        pEntManager->transforms[j].logY += knockbackStep;
                    }
                    else                                     // Bottom edge collision
                    {
                        pEntManager->transforms[i].logY += knockbackStep;
                        pEntManager->transforms[j].logY -= knockbackStep;
                    }
                }
                else
                {
                    if (a.x + a.w / 2 < b.x + b.w / 2) // Left edge collision
                    {
                        pEntManager->transforms[i].logX -= knockbackStep;
                        pEntManager->transforms[j].logX += knockbackStep;
                    }
                    else                                     // Right edge collision
                    {
                        pEntManager->transforms[i].logX += knockbackStep;
                        pEntManager->transforms[j].logX -= knockbackStep;
                    }
                }
            }
        }
    }
}
