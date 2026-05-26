#include "r_renderer.h"
#include "s_system.h"
#include "e_entity.h"
#include "typedefs.h"

void R_RenderLocation(map_t* pLocation, e_manager_t* pEntManager)
{
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - entitySpriteSize / 2);

	for (uint32_t y = 0; y < pLocation->rows; ++y)
	{
		for (uint32_t x = 0; x < pLocation->columns; ++x)
		{
			int screenX = pLocation->locationTiles[y * pLocation->columns + x].posX - pEntManager->transforms[0].logX + screenXCenter;
			int screenY = pLocation->locationTiles[y * pLocation->columns + x].posY;

			// Tiles culling
			if (screenX + TILE_SPRITE_SIZE * TILE_SPRITE_SCALE < 0
				|| screenX > LOGICAL_WIDTH
				|| screenY < 0
				|| screenY > LOGICAL_HEIGHT)
			{
				continue;
			}

			S_RenderLocation(pLocation, x, y, screenX, screenY);
		}
	}
}

void R_RenderObject(obj_manager_t* pObjManager, e_manager_t* pEntManager)
{
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - entitySpriteSize / 2);

	for (int i = 0; i < pObjManager->objCount; ++i)
	{
		int screenX = (int) pObjManager->transforms[i].logX - pEntManager->transforms[0].logX + screenXCenter; // Move relatively player
		int screenY = (int) pObjManager->transforms[i].logY;

		// Objects culling
		if (screenX + TILE_SPRITE_SIZE * TILE_SPRITE_SCALE < 0
			|| screenX > LOGICAL_WIDTH
			|| screenY < 0
			|| screenY > LOGICAL_HEIGHT)
		{
			continue;
		}

		if (pObjManager->isAnimated[i])
		{
			S_ReactionTimerStart(&pObjManager->animTimer[i]);

			if (S_IsTimeToReact(&pObjManager->animTimer[i]))
			{
				pObjManager->sprites[i].currentSprite = (pObjManager->sprites[i].currentSprite + 1) % OBJ_FRAMES_COUNT;
				pObjManager->sprites[i].srcX = TILE_SPRITE_SIZE * pObjManager->sprites[i].currentSprite;

				S_ReactionTimerEnd(&pObjManager->animTimer[i]);
			}
		}

		S_RenderObject(pObjManager, i, screenX, screenY);
	}
}

void R_RenderEntity(e_manager_t* pEntManager)
{
	int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - entitySpriteSize / 2);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		int screenX = i > 0 ? (int) (pEntManager->transforms[i].logX - pEntManager->transforms[0].logX + screenXCenter) : (int) screenXCenter;
		int screenY = (int) pEntManager->transforms[i].logY;

		// Entity culling
		if (screenX + entitySpriteSize < 0
			|| screenX > LOGICAL_WIDTH
			|| screenY < 0
			|| screenY > LOGICAL_HEIGHT)
		{
			E_MarkEntityToRemove(i, pEntManager);
			continue;
		}
		else S_ReactionTimerReset(&pEntManager->destructTimer[i]);

		switch (pEntManager->state[i])
		{
		case STATE_SPAWNING:
			R_Anim(pEntManager, i, SPAWN_FRAMES_COUNT, 1);
			break;
		case STATE_REMOVING:
			R_Anim_Death(pEntManager, i, DEATH_FRAMES_COUNT, 2);
			break;
		case STATE_ANGER:
			R_Anim(pEntManager, i, ANGER_FRAMES_COUNT, 3);
			break;
		default:
			R_Anim_Walk(pEntManager, i);
			break;
		}

		S_RenderEntity(pEntManager, i, screenX, screenY, pEntManager->transforms[i].flip);
	}
}

void R_Anim_Walk(e_manager_t* pEntManager, int i)
{
	if (pEntManager->isMoving[i])
	{
		S_ReactionTimerStart(&pEntManager->animTimer[i]);

		if (S_IsTimeToReact(&pEntManager->animTimer[i]))
		{
			pEntManager->sprites[i].currentSprite = (pEntManager->sprites[i].currentSprite + 1) % WALK_FRAMES_COUNT;
			pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
			pEntManager->sprites[i].srcY = 0;

			S_ReactionTimerEnd(&pEntManager->animTimer[i]);
		}
	}
	else
	{
		pEntManager->sprites[i].srcX = 0;
		pEntManager->sprites[i].srcY = 0;
	}
}

void R_Anim(e_manager_t* pEntManager, int i, int framesCount, int row)
{
	pEntManager->isMoving[i] = false;

	S_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (S_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= framesCount)
		{
			pEntManager->state[i] = STATE_NONE;
			pEntManager->sprites[i].currentSprite = 0;
			pEntManager->sprites[i].srcX = 0;
			pEntManager->sprites[i].srcY = 0;

			S_ReactionTimerEnd(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * row;

		S_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Death(e_manager_t* pEntManager, int i, int framesCount, int row)
{
	pEntManager->isMoving[i] = false;

	S_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (S_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= framesCount)
		{
			S_ReactionTimerEnd(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * row;

		S_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_RenderDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (!pGameState->isDebugMode) return;

	if (pGameState->deltaTime > 0)
		pGameState->currentFPS = (int)(1.0 / pGameState->deltaTime);

	char textBuffer[64];
	snprintf(
		textBuffer, sizeof(textBuffer), "FPS: %d | Entities: %d | x: %d | y: %d",
		pGameState->currentFPS, pEntManager->entitiesCount,
		(int) pEntManager->transforms[0].logX, (int) pEntManager->transforms[0].logY
	);

	S_RenderText(textBuffer, 100, 100, 255, 255, 255, 255);
}




