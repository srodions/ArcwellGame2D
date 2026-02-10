#ifndef R_RENDERER_H_
#define R_RENDERER_H_

#include "u_utility.h"
#include "p_physics.h"
#include "typedefs.h"

// --- DEFINITIONS ---
SDL_Window* R_WindowInit(gamestate_t* pGameState);
SDL_Renderer* R_RendererInit(SDL_Window* pWindow);
font_t R_FontInit();

void R_RenderLocation(SDL_Renderer* pRenderer, location_t* pLocation);
void R_RenderEntity(SDL_Renderer* pRenderer,  location_t* pLocation, e_manager_t* pEntity, gamestate_t* pGameState);
void R_RenderStats(SDL_Renderer* pRenderer, gamestate_t* pGameState, int* entitiesCount);

// --- IMPLEMENTATIONS ---
#if defined(STB_RENDERER_IMPLEMENTATION)

// --- GLOBAL VARIABLES ---
font_t font;
// --- PRIVATE VARIABLES ---
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
		"Arcwell Game 2D ver 0.06",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
	);

	return pWindow;
}

/*
 * This method creates a renderer and checks had been the renderer actually created.
 * If the renderer is NULL when this method tried to create it with the graphics card
 * acceleration, then it will try to create it with software (processor) rendering unit,
 * otherwise, method will send an error message, clear the SDL garbage and return NULL,
 * that need to be handled.
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
			SDL_DestroyWindow(pWindow);
			SDL_Quit();

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

void R_RenderLocation(SDL_Renderer* pRenderer, location_t* pLocation)
{
	for (int y = 0; y < pLocation->columns; ++y)
	{
		for (int x = 0; x < pLocation->rows; ++x)
		{
			// LOCATION SHIFT TO LEFT
			if (pLocation->isNextLocation)
				pLocation->locationTiles[y][x].posX -= camera_speed;

			pLocation->locationDest.x = pLocation->locationTiles[y][x].posX;
			pLocation->locationDest.y = pLocation->locationTiles[y][x].posY;
			pLocation->locationDest.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
			pLocation->locationDest.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;

			SDL_RenderCopy(
				pRenderer, pLocation->tileMap,
				&pLocation->locationTiles[y][x].tileSrc,
				&pLocation->locationDest
			);
		}
	}
}

/*
 * This is entity rendering function, that renders all of the entities and displaying them on the screen.
 */
void R_RenderEntity(SDL_Renderer* pRenderer, location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState)
{
	double dt = pGameState->deltaTime;

	for (int i = 0; i < pEntManager->entitiesCount; ++i)
	{
		switch (pEntManager->sprites[i].direction)
		{
		case 'L':
			pEntManager->sprites[i].flip = SDL_FLIP_HORIZONTAL;
			if (pEntManager->isMoving[i] && !pLocation->isNextLocation)
				pEntManager->transforms[i].logX -= player_speed * dt;
			break;
		case 'R':
			pEntManager->sprites[i].flip = SDL_FLIP_NONE;
			if (pEntManager->isMoving[i] && !pLocation->isNextLocation)
				pEntManager->transforms[i].logX += player_speed * dt;
			break;
		}

		// ENTITIES SHIFT TO LEFT
		if (pLocation->isNextLocation)
			pEntManager->transforms[i].logX -= camera_speed;

		pEntManager->entityDest.x = (int) pEntManager->transforms[i].logX;
		pEntManager->entityDest.y = (int) pEntManager->transforms[i].logY;
		pEntManager->entityDest.w = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
		pEntManager->entityDest.h = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;

		if (pEntManager->isMoving[i] && !pLocation->isNextLocation)
		{
			U_ReactionTimerStart(&pEntManager->animTimer[i]);

			if (U_IsTimeToReact(&pEntManager->animTimer[i]))
			{
				pEntManager->sprites[i].currentSprite = (pEntManager->sprites[i].currentSprite + 1) % SPRITE_COUNT;
				pEntManager->sprites[i].entitySrc.x = ENTITY_SPRITE_SIZE * pEntManager->sprites[i].currentSprite;

				U_ReactionTimerEnd(&pEntManager->animTimer[i]);
			}
		}
		else pEntManager->sprites[i].entitySrc.x = 0;

		SDL_RenderCopyEx(
			pRenderer, pEntManager->sprites[i].entityImg,
			&pEntManager->sprites[i].entitySrc, &pEntManager->entityDest,
			0.0, NULL, pEntManager->sprites[i].flip
		);
	}
}

void R_RenderStats(SDL_Renderer* pRenderer, gamestate_t* pGameState, int* pEntitiesCount)
{
	if (pGameState->isDebugMode)
	{
		U_ReactionTimerStart(&_debugUpdTimer);

		if (font.textTexture == NULL || U_IsTimeToReact(&_debugUpdTimer))
		{
			double dt = pGameState->deltaTime;

			if (dt > 0)
				pGameState->currentFPS = (int) (1.0 / dt);

			if (font.textTexture != NULL)
				SDL_DestroyTexture(font.textTexture);

			char textBuffer[32];
			snprintf(textBuffer, sizeof(textBuffer), "FPS: %d | Entities count: %d", pGameState->currentFPS, *pEntitiesCount);

			font.textSurface = TTF_RenderText_Blended(font.file, textBuffer, font.textColor);
			font.textTexture = SDL_CreateTextureFromSurface(pRenderer, font.textSurface);

			int textW = font.textSurface->w;
			int textH = font.textSurface->h;

			SDL_FreeSurface(font.textSurface);

			font.textRect.x = 100;
			font.textRect.y = 100;
			font.textRect.w = textW;
			font.textRect.h = textH;

			U_ReactionTimerEnd(&_debugUpdTimer);
		}

		SDL_RenderCopy(pRenderer, font.textTexture, NULL, &font.textRect);
	}
}
#endif /* STB_RENDERER_IMPLEMENTATION */

#endif /* R_RENDERER_H_ */
