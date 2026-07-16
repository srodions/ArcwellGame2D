#include "r_renderer.h"
#include "g_entity.h"
#include "i_system.h"
#include "typedefs.h"

void R_RenderLocation(map_t* pLocation, e_manager_t* pEntManager)
{
	const int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const int screenXCenter = LOGICAL_WIDTH / 2 - entitySpriteSize / 2;

	for (uint32_t y = 0; y < pLocation->rows; ++y)
	{
		for (uint32_t x = 0; x < pLocation->columns; ++x)
		{
			int screenX = pLocation->locationTiles[y * pLocation->columns + x].posX - FIXED_TO_INT(pEntManager->transforms[PLAYER].logX) + screenXCenter;
			int screenY = pLocation->locationTiles[y * pLocation->columns + x].posY;

			// Tiles culling
			if (screenX + TILE_SPRITE_SIZE * TILE_SPRITE_SCALE < 0
				|| screenX > LOGICAL_WIDTH
				|| screenY < 0
				|| screenY > LOGICAL_HEIGHT)
			{
				continue;
			}

			I_RenderLocation(pLocation, x, y, screenX, screenY);
		}
	}
}

void R_RenderObject(obj_manager_t* pObjManager, e_manager_t* pEntManager)
{
	const int entitySpriteSize = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	const int screenXCenter = LOGICAL_WIDTH / 2 - entitySpriteSize / 2;
	const fixed_t screenXCenterFixed = INT_TO_FIXED(screenXCenter);

	for (int i = 0; i < pObjManager->objCount; ++i)
	{
		int screenX = FIXED_TO_INT(pObjManager->transforms[i].logX - pEntManager->transforms[PLAYER].logX + screenXCenterFixed); // Move relatively player
		int screenY = FIXED_TO_INT(pObjManager->transforms[i].logY);

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
			I_ReactionTimerStart(&pObjManager->animTimer[i]);

			if (I_IsTimeToReact(&pObjManager->animTimer[i]))
			{
				pObjManager->sprites[i].currentSprite = (pObjManager->sprites[i].currentSprite + 1) % OBJ_FRAMES_COUNT;
				pObjManager->sprites[i].srcX = TILE_SPRITE_SIZE * pObjManager->sprites[i].currentSprite;

				I_ReactionTimerEnd(&pObjManager->animTimer[i]);
			}
		}

		I_RenderObject(pObjManager, i, screenX, screenY);
	}
}

void R_RenderEntity(e_manager_t* pEntManager)
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
			G_MarkEntityToRemove(i, pEntManager);
			continue;
		}
		else I_ReactionTimerReset(&pEntManager->destructTimer[i]);

		switch (pEntManager->state[i])
		{
		case STATE_SPAWNING:
			R_Anim_Spawn(pEntManager, &configs[pEntManager->id[i]], i);
			break;
		case STATE_REMOVING:
			R_Anim_Death(pEntManager, &configs[pEntManager->id[i]], i);
			break;
		case STATE_ANGER:
			R_Anim_Anger(pEntManager, &configs[pEntManager->id[i]], i);
			break;
		case STATE_ATTACK:
			R_Anim_Attack(pEntManager, &configs[pEntManager->id[i]], i);
			break;
		case STATE_HURT:
			// TODO: HURT ANIMATION
			break;
		default:
			R_Anim_Walk(pEntManager, &configs[pEntManager->id[i]], i);
			break;
		}

		I_RenderEntity(pEntManager, i, screenX, screenY, pEntManager->transforms[i].flip);
	}
}

void R_Anim_Attack(e_manager_t* pEntManager, e_config_t* config, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= config->attackFramesCount)
		{
			pEntManager->state[i] = STATE_NONE;
			pEntManager->sprites[i].currentSprite = 0;
			pEntManager->sprites[i].srcX = 0;
			pEntManager->sprites[i].srcY = 0;
			pEntManager->hasDamaged[i] = false;

			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->attackFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Walk(e_manager_t* pEntManager, e_config_t* config, int i)
{
	if (pEntManager->isMoving[i])
	{
		I_ReactionTimerStart(&pEntManager->animTimer[i]);

		if (I_IsTimeToReact(&pEntManager->animTimer[i]))
		{
			pEntManager->sprites[i].currentSprite = (pEntManager->sprites[i].currentSprite + 1) % config->walkFramesCount;
			pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
			pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->walkFramesRow;

			I_ReactionTimerEnd(&pEntManager->animTimer[i]);
		}
	}
	else
	{
		pEntManager->sprites[i].srcX = 0;
		pEntManager->sprites[i].srcY = 0;
	}
}

void R_Anim_Spawn(e_manager_t* pEntManager, e_config_t* config, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= config->spawnFramesCount)
		{
			pEntManager->state[i] = STATE_NONE;
			pEntManager->sprites[i].currentSprite = 0;
			pEntManager->sprites[i].srcX = 0;
			pEntManager->sprites[i].srcY = 0;

			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->spawnFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Anger(e_manager_t* pEntManager, e_config_t* config, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= config->angerFramesCount)
		{
			pEntManager->state[i] = STATE_NONE;
			pEntManager->sprites[i].currentSprite = 0;
			pEntManager->sprites[i].srcX = 0;
			pEntManager->sprites[i].srcY = 0;

			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->angerFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Death(e_manager_t* pEntManager, e_config_t* config, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= config->deathFramesCount)
		{
			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->deathFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

// TODO: Delete this and use bitmap atlas because of huge processor usage
void R_RenderDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (!pGameState->isDebugMode) return;

	pGameState->fpsTimer += pGameState->deltaTime;
	fixed_t updateInterval = FIXED_ONE / 4;

	if (pGameState->fpsTimer >= updateInterval)
	{
		pGameState->fpsTimer = 0;

		if (pGameState->deltaTime > 0)
			pGameState->currentFPS = (int)(FIXED_ONE / pGameState->deltaTime);

		snprintf(
			pGameState->debugText, sizeof(pGameState->debugText),
			"FPS: %d | Entities: %d | x: %d | y: %d | HP: %d",
			pGameState->currentFPS, pEntManager->entitiesCount,
			FIXED_TO_INT(pEntManager->transforms[PLAYER].logX),
			FIXED_TO_INT(pEntManager->transforms[PLAYER].logY),
			pEntManager->hp[PLAYER]
		);
	}

	if (pGameState->debugText[0] != '\0')
		I_RenderText(pGameState->debugText, 100, 100, 255, 255, 255, 255);
}
