#include "r_renderer.h"
#include "g_entity.h"
#include "l_arcloader.h"
#include "i_system.h"
#include <string.h>
#include <stdlib.h>

uint32_t r_screenBuffer[SCR_LOGICAL_WIDTH * SCR_LOGICAL_HEIGHT];
renderasset_t* r_mapAssets;
renderasset_t* r_objAssets;
renderasset_t* r_entAssets;
renderasset_t* r_uiAssets;

void R_LoadSpritesData(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, arcf_namesentry_t* pNamesHeader)
{
    uint32_t currentDataSize = 0;

    r_objAssets = (renderasset_t*) malloc(pNamesHeader->objCount * sizeof(renderasset_t));
    r_entAssets = (renderasset_t*) malloc(pNamesHeader->entCount * sizeof(renderasset_t));
    r_mapAssets = (renderasset_t*) malloc(pNamesHeader->mapCount * sizeof(renderasset_t));
    r_uiAssets = (renderasset_t*) malloc(pNamesHeader->uiCount * sizeof(renderasset_t));

    // OBJECTS
    char *objName = strtok(pNamesHeader->objNames, " ");

    for (int i = 0; i < pNamesHeader->objCount; ++i)
    {
    	if (objName == NULL) break;

		r_objAssets[i].rawData = L_LoadLump(arcFile, objName, pHeader, pTable, &currentDataSize);
		r_objAssets[i].header  = (arcf_spriteheader_t*) r_objAssets[i].rawData;
		r_objAssets[i].pixels  = (uint32_t*) (r_objAssets[i].header + 1);

		objName = strtok(NULL, " ");
    }

    // ENTITIES
    char *entName = strtok(pNamesHeader->entNames, " ");

    for (int i = 0; i < pNamesHeader->entCount; ++i)
    {
    	if (entName == NULL) break;

		r_entAssets[i].rawData = L_LoadLump(arcFile, entName, pHeader, pTable, &currentDataSize);
		r_entAssets[i].header  = (arcf_spriteheader_t*) r_entAssets[i].rawData;
		r_entAssets[i].pixels  = (uint32_t*) (r_entAssets[i].header + 1);

		entName = strtok(NULL, " ");
    }

    // MAPS
    char *mapName = strtok(pNamesHeader->mapNames, " ");

    for (int i = 0; i < pNamesHeader->mapCount; ++i)
	{
    	if (mapName == NULL) break;

		r_mapAssets[i].rawData = L_LoadLump(arcFile, mapName, pHeader, pTable, &currentDataSize);
		r_mapAssets[i].header  = (arcf_spriteheader_t*) r_mapAssets[i].rawData;
		r_mapAssets[i].pixels  = (uint32_t*) (r_mapAssets[i].header + 1);

		mapName = strtok(NULL, " ");
	}

    // UIs
    char *uiName = strtok(pNamesHeader->uiNames, " ");

    for (int i = 0; i < pNamesHeader->uiCount; ++i)
    {
        if (uiName == NULL) break;

        r_uiAssets[i].rawData = L_LoadLump(arcFile, uiName, pHeader, pTable, &currentDataSize);
        r_uiAssets[i].header  = (arcf_spriteheader_t*) r_uiAssets[i].rawData;
        r_uiAssets[i].pixels  = (uint32_t*) (r_uiAssets[i].header + 1);

        uiName = strtok(NULL, " ");
    }
}

void R_MoveSpriteToBuffer(const uint32_t* pixels, int spriteW, int spriteH, int posX, int posY)
{
    if (!pixels) return;

    int pixelIdx = 0;

    for (int y = 0; y < spriteH; ++y)
    {
        int screenY = posY + y;
        bool yInBounds = (screenY >= 0 && screenY < SCR_LOGICAL_HEIGHT);
        int screenRowOffset = screenY * SCR_LOGICAL_WIDTH;

        for (int x = 0; x < spriteW; ++x)
        {
            int screenX = posX + x;

            uint32_t pixel = pixels[pixelIdx++];

            if (yInBounds && screenX >= 0 && screenX < SCR_LOGICAL_WIDTH)
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
        bool yInBounds = (screenY >= 0 && screenY < SCR_LOGICAL_HEIGHT);	// Checks if Y is in bounds of the screen buffer
        int screenRowOffset = screenY * SCR_LOGICAL_WIDTH;

        int atlasRowOffset = (srcY + y) * atlasW;

        for (int x = 0; x < srcW; ++x)
        {
            int screenX = posX + x;
            int lookupX = flipX ? (srcW - 1 - x) : x;					// If flipX is true pixels will be read from right to left

            uint32_t pixel = pixels[atlasRowOffset + (srcX + lookupX)];	// Picks current pixel color - pixels[Row_Offset + (X_Start + X_Offset)]

            if (yInBounds && screenX >= 0 && screenX < SCR_LOGICAL_WIDTH)
            {
            	if ((pixel >> 24) != 0x00)								// Checks if higher byte (alpha channel from ARGB format) is not 0x00 (opacity isn't 0%)
                    r_screenBuffer[screenRowOffset + screenX] = pixel;
            }
        }
    }
}

void R_PushLocation(map_manager_t* pMapManager, int activeMapIdx, e_manager_t* pEntManager)
{
	const int screenXCenter = SCR_LOGICAL_WIDTH / 2 - ENT_SPR_SIZE / 2;
	const int screenYCenter = SCR_LOGICAL_HEIGHT / 2 - ENT_SPR_SIZE / 2;

	int playerX = FIXED_TO_INT(pEntManager->transforms[PLAYER].logX);
	int playerY = FIXED_TO_INT(pEntManager->transforms[PLAYER].logY);

	map_t currentMap = pMapManager->maps[activeMapIdx];
	int atlasIdx = currentMap.tileAtlasIdx;

	for (uint32_t y = 0; y < currentMap.rows; ++y)
	{
		for (uint32_t x = 0; x < currentMap.columns; ++x)
		{
			int srcX = currentMap.locationTiles[y * currentMap.columns + x].srcX;
			int srcY = currentMap.locationTiles[y * currentMap.columns + x].srcY;

			if (srcX < 0 || srcY < 0) continue;

			int screenX = currentMap.locationTiles[y * currentMap.columns + x].posX - playerX + screenXCenter;
			int screenY = currentMap.locationTiles[y * currentMap.columns + x].posY - playerY + screenYCenter;

			if (screenX + TILE_SPR_SIZE < 0
				|| screenX >= SCR_LOGICAL_WIDTH
				|| screenY + TILE_SPR_SIZE < 0
				|| screenY >= SCR_LOGICAL_HEIGHT)
			{
				continue;
			}

			int atlasW = r_mapAssets[atlasIdx].header->spriteW;

			R_MoveAtlasSpriteToBuffer(r_mapAssets[atlasIdx].pixels, atlasW, screenX, screenY, srcX, srcY, TILE_SPR_SIZE, TILE_SPR_SIZE, 0);
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
		int sprIndex = pObjManager->sprIndex[i];

		const int screenXCenter = SCR_LOGICAL_WIDTH / 2 - ENT_SPR_SIZE / 2;
		const int screenYCenter = SCR_LOGICAL_HEIGHT / 2 - ENT_SPR_SIZE / 2;

		int screenX = objX - playerX + screenXCenter; 					// Move relatively player
		int screenY = objY - playerY + screenYCenter;

		// Objects culling
		if (screenX + TILE_SPR_SIZE < 0
			|| screenX >= SCR_LOGICAL_WIDTH
			|| screenY + TILE_SPR_SIZE < 0
			|| screenY >= SCR_LOGICAL_HEIGHT)
		{
			continue;
		}

		if (pObjManager->isAnimated[i])
		{
			I_ReactionTimerStart(&pObjManager->animTimer[i]);

			if (I_IsTimeToReact(&pObjManager->animTimer[i]))
			{
				pObjManager->sprites[i].currentSprite = (pObjManager->sprites[i].currentSprite + 1) % TORCH_FRMS_COUNT;
				pObjManager->sprites[i].srcX = TILE_SPR_SIZE * pObjManager->sprites[i].currentSprite;

				I_ReactionTimerEnd(&pObjManager->animTimer[i]);
			}
		}

		int srcX = pObjManager->sprites[i].srcX;
		int srcY = pObjManager->sprites[i].srcY;
		int atlasW = r_objAssets[sprIndex].header->spriteW;

		R_MoveAtlasSpriteToBuffer(r_objAssets[sprIndex].pixels, atlasW, screenX, screenY, srcX, srcY, TILE_SPR_SIZE, TILE_SPR_SIZE, 0);
	}
}

void R_PushEntity(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager)
{
	int playerX = FIXED_TO_INT(pEntManager->transforms[PLAYER].logX);
	int playerY = FIXED_TO_INT(pEntManager->transforms[PLAYER].logY);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		int entityX = FIXED_TO_INT(pEntManager->transforms[i].logX);
		int entityY = FIXED_TO_INT(pEntManager->transforms[i].logY);
		int atlasIdx = pEntManager->atlasSprIdx[i];
		int entityFlip = pEntManager->transforms[i].flip;

		const int screenXCenter = SCR_LOGICAL_WIDTH / 2 - ENT_SPR_SIZE / 2;
		const int screenYCenter = SCR_LOGICAL_HEIGHT / 2 - ENT_SPR_SIZE / 2;

		int screenX = i > PLAYER ? entityX - playerX + screenXCenter : screenXCenter;
		int screenY = entityY - playerY + screenYCenter;

		// Entity culling
		if (screenX + ENT_SPR_SIZE < 0
			|| screenX >= SCR_LOGICAL_WIDTH
			|| screenY + ENT_SPR_SIZE < 0
			|| screenY >= SCR_LOGICAL_HEIGHT)
		{
			if (pEntManager->state[i] != STATE_SPAWNING)
				G_MarkEntityToRemove(i, pEntManager);
			continue;
		}
		else I_ReactionTimerReset(&pEntManager->destructTimer[i]);

		switch (pEntManager->state[i])
		{
		case STATE_SPAWNING:
			R_Anim_Spawn(pEntManager, pEntCfgManager, i);
			break;
		case STATE_REMOVING:
			R_Anim_Death(pEntManager, pEntCfgManager, i);
			break;
		case STATE_ANGER:
			R_Anim_Anger(pEntManager, pEntCfgManager, i);
			break;
		case STATE_ATTACK:
			R_Anim_Attack(pEntManager, pEntCfgManager, i);
			break;
		default:
			R_Anim_Walk(pEntManager, pEntCfgManager, i);
			break;
		}

		int srcX = pEntManager->sprites[i].srcX;
		int srcY = pEntManager->sprites[i].srcY;
		int atlasW = r_entAssets[atlasIdx].header->spriteW;

		R_MoveAtlasSpriteToBuffer(r_entAssets[atlasIdx].pixels, atlasW, screenX, screenY, srcX, srcY, ENT_SPR_SIZE, ENT_SPR_SIZE, entityFlip);
	}
}

void R_PushText(const char* text, int x, int y)
{
	int currentX = x;
	int atlasW = FONT_SPR_SIZE * FONT_SPR_PER_ROW;

	for (int i = 0; text[i] != '\0'; ++i)
	{
		unsigned char character = text[i];

		if (character < 32 || character > 127) continue;

		int index = character - 32;

		int srcX = (index % FONT_SPR_PER_ROW) * FONT_SPR_SIZE;
		int srcY = (index / FONT_SPR_PER_ROW) * FONT_SPR_SIZE;

		R_MoveAtlasSpriteToBuffer(r_uiAssets[FONT].pixels, atlasW, currentX, y, srcX, srcY, FONT_SPR_SIZE, FONT_SPR_SIZE, 0);

		currentX += FONT_SPR_SIZE;
	}
}

void R_PushUI(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	int yIndex = pEntManager->hp[PLAYER] * HP_BAR_SPR_H;
	R_MoveAtlasSpriteToBuffer(r_uiAssets[HEALTH_BAR].pixels, HP_BAR_SPR_W, 20, 20, 0, yIndex, HP_BAR_SPR_W, HP_BAR_SPR_H, 0);

	if (pGameState->isDebugMode)
		R_PushText(pGameState->debugText, 20, 10);

	if (pGameState->isPaused && pEntManager->hp[PLAYER] == 0)
		R_PushText("GAME OVER", 116, 80);
	else if (pGameState->isPaused)
		R_PushText("PAUSE", 140, 80);
}

void R_PushScene(gamestate_t* pGameState, map_manager_t* pMapManager, obj_manager_t* pObjManager, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager)
{
	memset(r_screenBuffer, 0, sizeof(r_screenBuffer));	// Clears screen buffer

	R_PushLocation(pMapManager, 0, pEntManager);
	R_PushObject(pObjManager, pEntManager);
	R_PushEntity(pEntManager, pEntCfgManager);
	R_PushUI(pGameState, pEntManager);
}

void R_Anim_Attack(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= pEntCfgManager->configs[i].attackFramesCount)
		{
			G_SetState(i, pEntManager, STATE_NONE);
			pEntManager->hasDamaged[i] = false;

			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENT_SPR_SIZE * pEntCfgManager->configs[i].attackFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Walk(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		pEntManager->sprites[i].currentSprite = pEntManager->isMoving[i]
			? (pEntManager->sprites[i].currentSprite + 1) % pEntCfgManager->configs[i].walkFramesCount
			: (pEntManager->sprites[i].currentSprite + 1) % pEntCfgManager->configs[i].stayFramesCount;

		pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;

		pEntManager->sprites[i].srcY = pEntManager->isMoving[i]
			? ENT_SPR_SIZE * pEntCfgManager->configs[i].walkFramesRow
			: ENT_SPR_SIZE * pEntCfgManager->configs[i].stayFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Spawn(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= pEntCfgManager->configs[i].spawnFramesCount)
		{
			G_SetState(i, pEntManager, STATE_NONE);
			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENT_SPR_SIZE * pEntCfgManager->configs[i].spawnFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Anger(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= pEntCfgManager->configs[i].angerFramesCount)
		{
			G_SetState(i, pEntManager, STATE_NONE);
			I_ReactionTimerReset(&pEntManager->animTimer[i]);
			return;
		}

		pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENT_SPR_SIZE * pEntCfgManager->configs[i].angerFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Anim_Death(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i)
{
	pEntManager->isMoving[i] = false;

	I_ReactionTimerStart(&pEntManager->animTimer[i]);

	if (I_IsTimeToReact(&pEntManager->animTimer[i]))
	{
		++(pEntManager->sprites[i].currentSprite);

		if (pEntManager->sprites[i].currentSprite >= pEntCfgManager->configs[i].deathFramesCount)
		{
			I_ReactionTimerReset(&pEntManager->animTimer[i]);

			int randomLastSprite = pEntManager->rdLastDeathSprites[i];

			pEntManager->sprites[i].currentSprite = (pEntCfgManager->configs[i].deathFramesCount - 1) + randomLastSprite;
			pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;
			pEntManager->sprites[i].srcY = ENT_SPR_SIZE * pEntCfgManager->configs[i].deathFramesRow;
			return;
		}

		pEntManager->sprites[i].srcX = ENT_SPR_SIZE * pEntManager->sprites[i].currentSprite;
		pEntManager->sprites[i].srcY = ENT_SPR_SIZE * pEntCfgManager->configs[i].deathFramesRow;

		I_ReactionTimerEnd(&pEntManager->animTimer[i]);
	}
}

void R_Destruct(arcf_namesentry_t* sprNamesHeader)
{
	for (int i = 0; i < sprNamesHeader->objCount; ++i)
	{
		if (r_objAssets[i].rawData != NULL)
		{
			free(r_objAssets[i].rawData);
			r_objAssets[i].rawData = NULL;
		}
		r_objAssets[i].header = NULL;
		r_objAssets[i].pixels = NULL;
	}

	for (int i = 0; i < sprNamesHeader->entCount; ++i)
	{
		if (r_entAssets[i].rawData != NULL)
		{
			free(r_entAssets[i].rawData);
			r_entAssets[i].rawData = NULL;
		}
		r_entAssets[i].header = NULL;
		r_entAssets[i].pixels = NULL;
	}

	for (int i = 0; i < sprNamesHeader->uiCount; ++i)
	{
		if (r_uiAssets[i].rawData != NULL)
		{
			free(r_uiAssets[i].rawData);
			r_uiAssets[i].rawData = NULL;
		}
		r_uiAssets[i].header = NULL;
		r_uiAssets[i].pixels = NULL;
	}

	for (int i = 0; i < sprNamesHeader->mapCount; ++i)
	{
		if (r_mapAssets[i].rawData != NULL)
		{
			free(r_mapAssets[i].rawData);
			r_mapAssets[i].rawData = NULL;
		}
		r_mapAssets[i].header = NULL;
		r_mapAssets[i].pixels = NULL;
	}
}
