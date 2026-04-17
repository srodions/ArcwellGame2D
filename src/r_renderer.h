#ifndef R_RENDERER_H_
#define R_RENDERER_H_

// --- DEFINITIONS ---
SDL_Window* R_WindowInit(gamestate_t* pGameState);
SDL_Renderer* R_RendererInit(SDL_Window* pWindow);
font_t R_FontInit();

void R_RenderLocation(SDL_Renderer* pRenderer, location_t* pLocation, e_manager_t* pEntManager);
void R_RenderObject(SDL_Renderer* pRenderer, location_t* pLocation, obj_manager_t* pObjManager, e_manager_t* pEntManager);
void R_RenderEntity(SDL_Renderer* pRenderer,  location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState);
void R_RenderStats(SDL_Renderer* pRenderer, gamestate_t* pGameState, e_manager_t* pEntManager);
void R_Destruct(SDL_Renderer* pRenderer, SDL_Window* pWindow);

// --- IMPLEMENTATIONS ---
#if defined(STB_RENDERER_IMPLEMENTATION)

font_t font;
rtimer_t _debugUpdTimer;

/*
 * This method creates an SDL Windows application window of the game.
 */
SDL_Window* R_WindowInit(gamestate_t* pGameState)
{
	SDL_DisplayMode dMode;

	if (SDL_GetDesktopDisplayMode(0, &dMode) == 0)
	{
		pGameState->screenW = dMode.w;
		pGameState->screenH = dMode.h;
		pGameState->targetFPS = dMode.refresh_rate;
		pGameState->targetFrameTime = 1.0 / (double) pGameState->targetFPS;
	}

	SDL_Window* pWindow = SDL_CreateWindow(
		"Arcwell Game 2D ver 0.09",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
	);

	return pWindow;
}

/*
 * This method creates a renderer and checks had been the renderer actually created.
 * If the renderer is NULL when this method tried to create it with the graphics card
 * acceleration, then it will try to create it with software renderer, otherwise,
 * method will send an error message and return NULL, that need to be handled in main().
 */
SDL_Renderer* R_RendererInit(SDL_Window* pWindow)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); // Avoiding scale blur
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

	if (pRenderer == NULL)
	{
		pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_SOFTWARE);
		if (pRenderer == NULL)
		{
			printf("SDL_CreateRenderer Error: %s, ensure that your video drivers had been installed!\n", SDL_GetError());
			return NULL;
		}
	}

	SDL_RenderSetLogicalSize(pRenderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);

	font = R_FontInit();
	_debugUpdTimer.reactionTime = 1000;

	return pRenderer;
}

font_t R_FontInit()
{
	font_t font = {
		.file = TTF_OpenFont("res/font/x12y16pxMaruMonica.ttf", 40),
		.textColor.r = 255,
		.textColor.g = 255,
		.textColor.b = 255,
		.textColor.a = 255
	};

	return font;
}

/*
 * TODO: Make it to not render tiles when they out of monitor screen
 */
void R_RenderLocation(SDL_Renderer* pRenderer, location_t* pLocation, e_manager_t* pEntManager)
{
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - pEntManager->entityDest.w / 2);

	for (uint32_t y = 0; y < pLocation->rows; ++y)
	{
		for (uint32_t x = 0; x < pLocation->columns; ++x)
		{
			pLocation->locationDest.x = pLocation->locationTiles[y * pLocation->columns + x].posX - pEntManager->transforms[0].logX + screenXCenter; // Move relatively player
			pLocation->locationDest.y = pLocation->locationTiles[y * pLocation->columns + x].posY;

			SDL_RenderCopy(
				pRenderer, pLocation->tileMap,
				&pLocation->locationTiles[y * pLocation->columns + x].tileSrc,
				&pLocation->locationDest
			);
		}
	}
}

/*
 * TODO: Make it to not render objects when they out of monitor screen
 */
void R_RenderObject(SDL_Renderer* pRenderer, location_t* pLocation, obj_manager_t* pObjManager, e_manager_t* pEntManager)
{
	const float screenXCenter = (float)(LOGICAL_WIDTH / 2 - pEntManager->entityDest.w / 2);

	for (int i = 0; i < pObjManager->objCount; ++i)
	{
		pObjManager->objDest.x = (int) pObjManager->transforms[i].logX - pEntManager->transforms[0].logX + screenXCenter; // Move relatively player
		pObjManager->objDest.y = (int) pObjManager->transforms[i].logY;

		if (pObjManager->isAnimated[i])
		{
			U_ReactionTimerStart(&pObjManager->animTimer[i]);

			if (U_IsTimeToReact(&pObjManager->animTimer[i]))
			{
				pObjManager->sprites[i].currentSprite = (pObjManager->sprites[i].currentSprite + 1) % OBJ_FRAMES_COUNT;
				pObjManager->sprites[i].spriteSrc.x = TILE_SPRITE_SIZE * pObjManager->sprites[i].currentSprite;

				U_ReactionTimerEnd(&pObjManager->animTimer[i]);
			}
		}

		SDL_RenderCopy(
			pRenderer, pObjManager->sprites[i].spriteImg,
			&pObjManager->sprites[i].spriteSrc, &pObjManager->objDest
		);
	}
}

/*
 * TODO: Make it to not render entities (or just remove them) when they out of monitor screen
 */
void R_RenderEntity(SDL_Renderer* pRenderer, location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState)
{
	double dt = pGameState->deltaTime;
	const int screenXCenter = LOGICAL_WIDTH / 2 - pEntManager->entityDest.w / 2;

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		switch (pEntManager->sprites[i].direction)
		{
		case LEFT:
			pEntManager->sprites[i].flip = SDL_FLIP_HORIZONTAL;
			if (pEntManager->isMoving[i])
				pEntManager->transforms[i].logX -= player_speed * dt;
			break;
		case RIGHT:
			pEntManager->sprites[i].flip = SDL_FLIP_NONE;
			if (pEntManager->isMoving[i])
				pEntManager->transforms[i].logX += player_speed * dt;
			break;
		}

		if (i > 0)
			pEntManager->entityDest.x = (int) pEntManager->transforms[i].logX - pEntManager->transforms[0].logX; // For each exclude player (Move relatively player)
		else
			pEntManager->entityDest.x = screenXCenter; // For player (centered on the screen entity)

		pEntManager->entityDest.y = (int) pEntManager->transforms[i].logY;

		if (pEntManager->isMoving[i])
		{
			U_ReactionTimerStart(&pEntManager->animTimer[i]);

			if (U_IsTimeToReact(&pEntManager->animTimer[i]))
			{
				pEntManager->sprites[i].currentSprite = (pEntManager->sprites[i].currentSprite + 1) % ENTITY_FRAMES_COUNT;
				pEntManager->sprites[i].spriteSrc.x = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;

				U_ReactionTimerEnd(&pEntManager->animTimer[i]);
			}
		}
		else pEntManager->sprites[i].spriteSrc.x = 0;

		SDL_RenderCopyEx(
			pRenderer, pEntManager->sprites[i].spriteImg,
			&pEntManager->sprites[i].spriteSrc, &pEntManager->entityDest,
			0.0, NULL, pEntManager->sprites[i].flip
		);
	}
}

void R_RenderStats(SDL_Renderer* pRenderer, gamestate_t* pGameState, e_manager_t* entManager)
{
	if (!pGameState->isDebugMode) return;

	U_ReactionTimerStart(&_debugUpdTimer);

	if (font.textTexture == NULL || U_IsTimeToReact(&_debugUpdTimer))
	{
		if (pGameState->deltaTime > 0)
			pGameState->currentFPS = (int)(1.0 / pGameState->deltaTime);

		if (font.textTexture != NULL) {
			SDL_DestroyTexture(font.textTexture);
			font.textTexture = NULL;
		}

		char textBuffer[64];
		snprintf(textBuffer, sizeof(textBuffer), "FPS: %d | Entities: %d | x: %d | y: %d",
			pGameState->currentFPS, entManager->entitiesCount,
			(int) entManager->transforms[0].logX, (int) entManager->transforms[0].logY
		);

		SDL_Surface* tempSurface = TTF_RenderText_Blended(font.file, textBuffer, font.textColor);
		if (tempSurface != NULL)
		{
			font.textTexture = SDL_CreateTextureFromSurface(pRenderer, tempSurface);

			font.textRect.x = 100;
			font.textRect.y = 100;
			font.textRect.w = tempSurface->w;
			font.textRect.h = tempSurface->h;

			SDL_FreeSurface(tempSurface);
		}

		U_ReactionTimerEnd(&_debugUpdTimer);
	}

	if (font.textTexture != NULL)
		SDL_RenderCopy(pRenderer, font.textTexture, NULL, &font.textRect);
}

/*
 * Destructor method to clean up all renderer textures, close files and quit the SDL
 * (Always need to be called in application crash or normal exit!!!)
 */
void R_Destruct(SDL_Renderer* pRenderer, SDL_Window* pWindow)
{
	if (font.textTexture != NULL)
	{
		SDL_DestroyTexture(font.textTexture);
		font.textTexture = NULL;
	}

	if (font.textSurface != NULL)
	{
		SDL_FreeSurface(font.textSurface);
		font.textSurface = NULL;
	}

	if (font.file != NULL)
	{
		TTF_CloseFont(font.file);
		font.file = NULL;
	}

	if (pRenderer != NULL)
		SDL_DestroyRenderer(pRenderer);

	if (pWindow != NULL)
		SDL_DestroyWindow(pWindow);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
#endif /* STB_RENDERER_IMPLEMENTATION */

#endif /* R_RENDERER_H_ */
