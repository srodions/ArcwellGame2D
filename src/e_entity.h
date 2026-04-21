#ifndef E_ENTITY_H_
#define E_ENTITY_H_

/* --- DEFINITIONS --- */
void E_EntitySpritesInit(SDL_Renderer* pRenderer);
void E_EntityInit(e_manager_t* pEntManager, int posX, int posY, float speed, enum ENTITY_ID id);
void E_MarkEntityToRemove(int index, e_manager_t* pEntManager);
void E_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager);
void E_EntityWallCollisionCheck(location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState);
void E_EntityToEntityCollisionCheck(e_manager_t* pEntManager, gamestate_t* pGameState);
void E_AI_Chase(e_manager_t* pEntManager);
void E_AI_Idle(e_manager_t* pEntManager);
void E_Destruct(e_manager_t* pEntManager);

/* --- IMPLEMENTATIONS --- */
#if defined(STB_ENTITY_IMPLEMENTATION)

static SDL_Texture* entity_sprites[MAX_ENTITY_SPRITES];

void E_EntitySpritesInit(SDL_Renderer* pRenderer)
{
	entity_sprites[0] = IMG_LoadTexture(pRenderer, "res/entity/player.png");
	entity_sprites[1] = IMG_LoadTexture(pRenderer, "res/entity/skeleton.png");
}

/*
 * This function initializes entity (data-oriented style).
 */
void E_EntityInit(e_manager_t* pEntManager, int posX, int posY, float speed, enum ENTITY_ID id)
{
	int i = pEntManager->entitiesCount;
	assert(i <= MAX_ENTITIES);
	// Texture load
	pEntManager->id[i] = id;
	pEntManager->sprites[i].spriteImg = entity_sprites[id];
	pEntManager->sprites[i].spriteSrc.x = 0;
	pEntManager->sprites[i].spriteSrc.y = 0;
	pEntManager->sprites[i].spriteSrc.w = ENTITY_SPRITE_SIZE;
	pEntManager->sprites[i].spriteSrc.h = ENTITY_SPRITE_SIZE;
	pEntManager->entityDest.w = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	pEntManager->entityDest.h = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	// Sprite controls
	pEntManager->sprites[i].direction = RIGHT;
	pEntManager->sprites[i].currentSprite = 0;
	pEntManager->transforms[i].logX = (float) posX;
	pEntManager->transforms[i].logY = (float) posY;
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

void E_MarkEntityToRemove(int index, e_manager_t* pEntManager)
{
	U_ReactionTimerStart(&pEntManager->destructTimer[index]);

	if (U_IsTimeToReact(&pEntManager->destructTimer[index]))
	{
		U_ReactionTimerEnd(&pEntManager->destructTimer[index]);
		E_RemoveEntityFromLoadList(index, pEntManager);
	}
}

/*
 * This function removes the entity by its index from all of the data arrays
 * and shifts all the data to the left from removed entity's index.
 */
void E_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager)
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

/*
 * This method checks entity wall collision in all of directions (left, right)
 */
void E_EntityWallCollisionCheck(location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState)
{
	const float mapWidth = (float)(pLocation->columns * TILE_SPRITE_SIZE * TILE_SPRITE_SCALE);
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - pEntManager->entityDest.w / 2);

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING) continue;

		if (pEntManager->transforms[i].logX < screenXCenter)
		{
			pEntManager->transforms[i].logX = screenXCenter;
			pEntManager->aiParams[i].isCollisionOnLeft = true;
		}
		else pEntManager->aiParams[i].isCollisionOnLeft = false;

		if (pEntManager->transforms[i].logX > mapWidth / 2 + screenXCenter / 2 + pEntManager->entityDest.w * 2)
		{
			pEntManager->transforms[i].logX = mapWidth / 2 + screenXCenter / 2 + pEntManager->entityDest.w * 2;
			pEntManager->aiParams[i].isCollisionOnRight = true;
		}
		else pEntManager->aiParams[i].isCollisionOnRight = false;
	}
}

void E_EntityToEntityCollisionCheck(e_manager_t* pEntManager, gamestate_t* pGameState)
{
	int eCount = pEntManager->entitiesCount;
	if (eCount < 2) return;

	double dt = pGameState->deltaTime;
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - pEntManager->entityDest.w / 2);

	for (int i = 0; i < eCount; ++i)
	{
		for (int j = i + 1; j < eCount; ++j)
		{
			if (pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING
				|| pEntManager->state[j] == STATE_SPAWNING || pEntManager->state[j] == STATE_REMOVING) continue;

			SDL_Rect a, b, result;

			// Calculating collision from the center of the screen if opposite entity is player
			a.x = (int) (j > 0 ? pEntManager->transforms[i].logX + screenXCenter : (pEntManager->transforms[i].logX + screenXCenter));
			a.y = (int) pEntManager->transforms[i].logY;
			a.w = pEntManager->transforms[i].hitboxW;
			a.h = pEntManager->transforms[i].hitboxH;
			// Calculating collision from the center of the screen if opposite entity is player
			b.x = (int) (i > 0 ? pEntManager->transforms[j].logX + screenXCenter : (pEntManager->transforms[j].logX + screenXCenter));
			b.y = (int) pEntManager->transforms[j].logY;
			b.w = pEntManager->transforms[j].hitboxW;
			b.h = pEntManager->transforms[j].hitboxH;

			if (SDL_IntersectRect(&a, &b, &result))
			{
				if (result.w >= result.h)
				{
					if (a.y + a.h / 2 < b.y + b.h / 2) 	// Top edge collision
					{
						pEntManager->transforms[i].logY -= knockback_strength * dt;
						pEntManager->transforms[j].logY += knockback_strength * dt;
					}
					else								// Bottom edge collision
					{
						pEntManager->transforms[i].logY += knockback_strength * dt;
						pEntManager->transforms[j].logY -= knockback_strength * dt;
					}
				}
				else
				{
					if (a.x + a.w / 2 < b.x + b.w / 2)	// Left edge collision
					{
						pEntManager->transforms[i].logX -= knockback_strength * dt;
						pEntManager->transforms[j].logX += knockback_strength * dt;
					}
					else								// Right edge collision
					{
						pEntManager->transforms[i].logX += knockback_strength * dt;
						pEntManager->transforms[j].logX -= knockback_strength * dt;
					}
				}
			}
		}
	}
}

/*
 * De-structor method to clean up all entities' textures before exit
 * (Always need to be called in application crash or normal exit!!!)
 */
void E_Destruct(e_manager_t* pEntManager)
{
	for (int i = 0; i < MAX_ENTITY_SPRITES; ++i)
	{
		if (entity_sprites[i] != NULL)
		{
			SDL_DestroyTexture(entity_sprites[i]);
			entity_sprites[i] = NULL;
		}
	}

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
		pEntManager->sprites[i].spriteImg = NULL;
}
#endif /* STB_ENTITY_IMPLEMENTATION */

#if defined(STB_ENTITY_AI_IMPLEMENTATION)
void E_AI_Idle(e_manager_t* pEntManager)
{
	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		if (pEntManager->ai[i] != AI_IDLE
			|| pEntManager->state[i] == STATE_SPAWNING || pEntManager->state[i] == STATE_REMOVING) continue;

		pEntManager->isMoving[i] = true;

		if (pEntManager->aiParams[i].isCollisionOnLeft)
			pEntManager->sprites[i].direction = RIGHT;
		else if (pEntManager->aiParams[i].isCollisionOnRight)
			pEntManager->sprites[i].direction = LEFT;

		U_ReactionTimerStart(&pEntManager->aiTimer[i]);

		if (U_IsTimeToReact(&pEntManager->aiTimer[i]))
		{
			pEntManager->aiTimer[i].reactionTime = AI_IDLE_MIN_RENEW_TIME + rand() % AI_IDLE_MAX_RENEW_TIME;
			pEntManager->aiParams[i].currentChoice = rand() % 100;

			if (pEntManager->aiParams[i].currentChoice <= 50)
				pEntManager->sprites[i].direction = LEFT;
			else
				pEntManager->sprites[i].direction = RIGHT;

			U_ReactionTimerEnd(&pEntManager->aiTimer[i]);
		}
	}
}
#endif /* STB_ENTITY_AI_IMPLEMENTATION */

#endif /* E_ENTITY_H_ */
