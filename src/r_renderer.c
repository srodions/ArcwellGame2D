#include "r_renderer.h"
#include "g_entity.h"
#include "l_arcloader.h"
#include "i_system.h"

uint32_t r_screenBuffer[LOGICAL_WIDTH * LOGICAL_HEIGHT];
renderasset_t r_mapAssets[MAX_SPRITES];
renderasset_t r_objAssets[MAX_SPRITES];
renderasset_t r_entAssets[MAX_SPRITES];

void R_LoadSpritesData(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable)
{
    uint32_t currentDataSize = 0;

    r_mapAssets[TOMB].rawData = L_LoadLump(arcFile, "TILES", pHeader, pTable, &currentDataSize);
    r_mapAssets[TOMB].header  = (arcf_spriteheader_t*) r_mapAssets[TOMB].rawData;
    r_mapAssets[TOMB].pixels  = (uint32_t*) (r_mapAssets[TOMB].header + 1);

    r_entAssets[PLAYER].rawData = L_LoadLump(arcFile, "PLAYER", pHeader, pTable, &currentDataSize);
    r_entAssets[PLAYER].header  = (arcf_spriteheader_t*) r_entAssets[PLAYER].rawData;
    r_entAssets[PLAYER].pixels  = (uint32_t*) (r_entAssets[PLAYER].header + 1);

    r_entAssets[SKELETON].rawData = L_LoadLump(arcFile, "SKELETON", pHeader, pTable, &currentDataSize);
    r_entAssets[SKELETON].header  = (arcf_spriteheader_t*) r_entAssets[SKELETON].rawData;
    r_entAssets[SKELETON].pixels  = (uint32_t*) (r_entAssets[SKELETON].header + 1);

    r_objAssets[TORCH].rawData = L_LoadLump(arcFile, "TORCH", pHeader, pTable, &currentDataSize);
    r_objAssets[TORCH].header  = (arcf_spriteheader_t*) r_objAssets[TORCH].rawData;
    r_objAssets[TORCH].pixels  = (uint32_t*) (r_objAssets[TORCH].header + 1);

    r_objAssets[DECORATION].rawData = L_LoadLump(arcFile, "DECORATION", pHeader, pTable, &currentDataSize);
    r_objAssets[DECORATION].header  = (arcf_spriteheader_t*) r_objAssets[DECORATION].rawData;
    r_objAssets[DECORATION].pixels  = (uint32_t*) (r_objAssets[DECORATION].header + 1);

    r_objAssets[CHEST].rawData = L_LoadLump(arcFile, "CHEST", pHeader, pTable, &currentDataSize);
    r_objAssets[CHEST].header  = (arcf_spriteheader_t*) r_objAssets[CHEST].rawData;
    r_objAssets[CHEST].pixels  = (uint32_t*) (r_objAssets[CHEST].header + 1);
}

void R_MoveSpriteToBuffer(const uint32_t* pixels, int spriteW, int spriteH, int posX, int posY)
{
    if (!pixels) return;

    int pixelIdx = 0;

    for (int y = 0; y < spriteH; ++y)
    {
        int screenY = posY + y;
        bool yInBounds = (screenY >= 0 && screenY < LOGICAL_HEIGHT);
        int screenRowOffset = screenY * LOGICAL_WIDTH;

        for (int x = 0; x < spriteW; ++x)
        {
            int screenX = posX + x;

            uint32_t pixel = pixels[pixelIdx++];

            if (yInBounds && screenX >= 0 && screenX < LOGICAL_WIDTH)
            {
            	if ((pixel >> 24) != 0x00)
                    r_screenBuffer[screenRowOffset + screenX] = pixel;
            }
        }
    }
}

void R_MoveAtlasSpriteToBuffer(const uint32_t* pixels, int atlasW, int posX, int posY, int srcX, int srcY, int srcW, int srcH, int flipX)
{
    if (!pixels) return;

    for (int y = 0; y < srcH; ++y)
    {
        int screenY = posY + y;
        bool yInBounds = (screenY >= 0 && screenY < LOGICAL_HEIGHT);
        int screenRowOffset = screenY * LOGICAL_WIDTH;

        int atlasRowOffset = (srcY + y) * atlasW;

        for (int x = 0; x < srcW; ++x)
        {
            int screenX = posX + x;
            int lookupX = flipX ? (srcW - 1 - x) : x;

            uint32_t pixel = pixels[atlasRowOffset + (srcX + lookupX)];

            if (yInBounds && screenX >= 0 && screenX < LOGICAL_WIDTH)
            {
            	if ((pixel >> 24) != 0x00)
                    r_screenBuffer[screenRowOffset + screenX] = pixel;
            }
        }
    }
}


void R_PushLocation(map_t* pLocation, e_manager_t* pEntManager)
{
	enum MAP_ID locationId = pLocation->id;

	const int screenXCenter = LOGICAL_WIDTH / 2 - ENTITY_SPRITE_SIZE / 2;
	const int screenYCenter = LOGICAL_HEIGHT / 2 - ENTITY_SPRITE_SIZE / 2;

	int playerX = FIXED_TO_INT(pEntManager->transforms[PLAYER].logX);
	int playerY = FIXED_TO_INT(pEntManager->transforms[PLAYER].logY);

	for (uint32_t y = 0; y < pLocation->rows; ++y)
	{
		for (uint32_t x = 0; x < pLocation->columns; ++x)
		{
			int srcX = pLocation->locationTiles[y * pLocation->columns + x].srcX;
			int srcY = pLocation->locationTiles[y * pLocation->columns + x].srcY;

			if (srcX < 0 || srcY < 0) continue;

			int screenX = pLocation->locationTiles[y * pLocation->columns + x].posX - playerX + screenXCenter;
			int screenY = pLocation->locationTiles[y * pLocation->columns + x].posY - playerY + screenYCenter;

			if (screenX + TILE_SPRITE_SIZE < 0
				|| screenX >= LOGICAL_WIDTH
				|| screenY + TILE_SPRITE_SIZE < 0
				|| screenY >= LOGICAL_HEIGHT)
			{
				continue;
			}

			int atlasW = r_mapAssets[locationId].header->spriteW;

			R_MoveAtlasSpriteToBuffer(r_mapAssets[locationId].pixels, atlasW, screenX, screenY, srcX, srcY, TILE_SPRITE_SIZE, TILE_SPRITE_SIZE, 0);
		}
	}
}

void R_PushObject(obj_manager_t* pObjManager, e_manager_t* pEntManager)
{
	int playerX = FIXED_TO_INT(pEntManager->transforms[PLAYER].logX);
	int playerY = FIXED_TO_INT(pEntManager->transforms[PLAYER].logY);

	for (int i = 0; i < pObjManager->objCount; ++i)
	{
		int objX = FIXED_TO_INT(pObjManager->transforms[i].logX);
		int objY = FIXED_TO_INT(pObjManager->transforms[i].logY);
		int objId = pObjManager->id[i];

		const int screenXCenter = LOGICAL_WIDTH / 2 - ENTITY_SPRITE_SIZE / 2;
		const int screenYCenter = LOGICAL_HEIGHT / 2 - ENTITY_SPRITE_SIZE / 2;

		int screenX = objX - playerX + screenXCenter; 					// Move relatively player
		int screenY = objY - playerY + screenYCenter;

		// Objects culling
		if (screenX + TILE_SPRITE_SIZE < 0
			|| screenX >= LOGICAL_WIDTH
			|| screenY + TILE_SPRITE_SIZE < 0
			|| screenY >= LOGICAL_HEIGHT)
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

		int srcX = pObjManager->sprites[i].srcX;
		int srcY = pObjManager->sprites[i].srcY;
		int atlasW = r_objAssets[objId].header->spriteW;

		R_MoveAtlasSpriteToBuffer(r_objAssets[objId].pixels, atlasW, screenX, screenY, srcX, srcY, TILE_SPRITE_SIZE, TILE_SPRITE_SIZE, 0);
	}
}

void R_PushEntity(e_manager_t* pEntManager)
{
	int playerX = FIXED_TO_INT(pEntManager->transforms[PLAYER].logX);
	int playerY = FIXED_TO_INT(pEntManager->transforms[PLAYER].logY);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		int entityX = FIXED_TO_INT(pEntManager->transforms[i].logX);
		int entityY = FIXED_TO_INT(pEntManager->transforms[i].logY);
		int entityId = pEntManager->id[i];
		int entityFlip = pEntManager->transforms[i].flip;

		const int screenXCenter = LOGICAL_WIDTH / 2 - ENTITY_SPRITE_SIZE / 2;
		const int screenYCenter = LOGICAL_HEIGHT / 2 - ENTITY_SPRITE_SIZE / 2;

		int screenX = i > PLAYER ? entityX - playerX + screenXCenter : screenXCenter;
		int screenY = entityY - playerY + screenYCenter;

		// Entity culling
		if (screenX + ENTITY_SPRITE_SIZE < 0
			|| screenX >= LOGICAL_WIDTH
			|| screenY + ENTITY_SPRITE_SIZE < 0
			|| screenY >= LOGICAL_HEIGHT)
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

		int srcX = pEntManager->sprites[i].srcX;
		int srcY = pEntManager->sprites[i].srcY;
		int atlasW = r_entAssets[entityId].header->spriteW;

		R_MoveAtlasSpriteToBuffer(r_entAssets[entityId].pixels, atlasW, screenX, screenY, srcX, srcY, ENTITY_SPRITE_SIZE, ENTITY_SPRITE_SIZE, entityFlip);
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
			pEntManager->sprites[i].currentSprite = config->deathFramesCount - 1;
			pEntManager->sprites[i].srcX = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;
			pEntManager->sprites[i].srcY = ENTITY_SPRITE_SIZE * config->deathFramesRow;
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

	if (pGameState->debugText[0] != '\0')
		I_RenderText(pGameState, pGameState->debugText, 25, 10, 255, 255, 255, 255);
}
