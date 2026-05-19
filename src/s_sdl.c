#if defined(SDL2_X64_WINDOWS)

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "s_system.h"
#include "g_map.h"
#include "h_keyboard.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "typedefs.h"

typedef struct Keymap
{
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode space;
    SDL_Scancode use;
    SDL_Scancode debug;
    SDL_Scancode remove;
    SDL_Scancode exit;
} keymap_t;

typedef struct Font
{
	TTF_Font* file;
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;

	SDL_Color textColor;
	SDL_Rect textRect;
} font_t;

// INPUT
SDL_Scancode input_keyScancode;
keymap_t keyMap;
// RENDERER
SDL_Window* pWindow;
SDL_Renderer* pRenderer;
font_t font;
// TEXTURES
SDL_Texture* entity_sprites[MAX_SPRITES];
SDL_Texture* obj_sprites[MAX_SPRITES];
SDL_Texture* tilemap_sprite;

/*
 * Starts the timer from current number of milliseconds since SDL library initialization.
 */
void S_ReactionTimerStart(rtimer_t* pReactionTimer)
{
	pReactionTimer->currentTime = SDL_GetTicks();
}

void S_ReactionTimerEnd(rtimer_t* pReactionTimer)
{
	pReactionTimer->lastTime = pReactionTimer->currentTime;
}

void S_ReactionTimerReset(rtimer_t* pReactionTimer)
{
    pReactionTimer->lastTime = SDL_GetTicks();
}

/*
 * Checks whether is delta time reached reaction time.
 */
bool S_IsTimeToReact(rtimer_t* pReactionTimer)
{
	return pReactionTimer->currentTime - pReactionTimer->lastTime >= pReactionTimer->reactionTime;
}

/*
 * This method initializes key map setup for controls.
 */
void S_InitKeymap()
{
    keyMap.up = SDL_SCANCODE_W;
    keyMap.down = SDL_SCANCODE_S;
    keyMap.left = SDL_SCANCODE_A;
    keyMap.right = SDL_SCANCODE_D;
    keyMap.space = SDL_SCANCODE_SPACE;
    keyMap.use = SDL_SCANCODE_E;
    keyMap.debug = SDL_SCANCODE_T;
    keyMap.remove = SDL_SCANCODE_R;
    keyMap.exit = SDL_SCANCODE_ESCAPE;
}

/*
 * This method handles keyboard input for setting keys' states true or false depending on
 * whether these keys pressed (down) or released (up).
 */
void S_HandleKeyboardInput(enum KBD_KEY_STATE keyState, keymap_t* keyMap, keystates_t* keyStates)
{
	if (input_keyScancode == keyMap->up)
		keyStates->isUp = keyState;
	else if (input_keyScancode == keyMap->down)
		keyStates->isDown = keyState;

	if (input_keyScancode == keyMap->left)
		keyStates->isLeft = keyState;
	else if (input_keyScancode == keyMap->right)
		keyStates->isRight = keyState;

	if (input_keyScancode == keyMap->exit)
		keyStates->isExit = keyState;
	if (input_keyScancode == keyMap->space)
		keyStates->isSpace = keyState;
	if (input_keyScancode == keyMap->use)
		keyStates->isUse = keyState;
	if (input_keyScancode == keyMap->debug)
		keyStates->isDebug = keyState;
	if (input_keyScancode == keyMap->remove)
		keyStates->isRemove = keyState;
}

/*
 * This function handles events that come to the SDL window and stay in queue.
 * These events can be: application's quit button event, any key pressed event,
 * any key released event, etc.
 */
void S_HandleEvents(gamestate_t *pGameState, e_manager_t* pEntManager, keystates_t* keyStates)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		pGameState->isRunning = false;
		break;
	case SDL_KEYDOWN:
		input_keyScancode = event.key.keysym.scancode;
		S_HandleKeyboardInput(KEY_STATE_DOWN, &keyMap, keyStates);
		break;
	case SDL_KEYUP:
		input_keyScancode = event.key.keysym.scancode;
		S_HandleKeyboardInput(KEY_STATE_UP, &keyMap, keyStates);
		break;
	}
}

void S_FrameStart(int* frameStart)
{
    *frameStart = SDL_GetTicks();
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);
}

void S_FrameEnd(gamestate_t* pGameState, int* frameStart)
{
	SDL_RenderPresent(pRenderer);
    pGameState->deltaTime = (SDL_GetTicks() - (*frameStart)) / 1000.0;

    if (pGameState->deltaTime < pGameState->targetFrameTime)
    {
        SDL_Delay((pGameState->targetFrameTime - pGameState->deltaTime) * 1000.0);
        pGameState->deltaTime = pGameState->targetFrameTime;
    }
}

int S_LibInit()
{
	if (IMG_Init(IMG_INIT_PNG) < 0 || TTF_Init() < 0)
		return -1;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
		return -1;

	return 0;
}

/*
 * This method creates an SDL Windows application window of the game.
 */
int S_WindowInit(gamestate_t* pGameState)
{
	SDL_DisplayMode dMode;

	if (SDL_GetDesktopDisplayMode(0, &dMode) == 0)
	{
		pGameState->screenW = dMode.w;
		pGameState->screenH = dMode.h;
		pGameState->targetFPS = dMode.refresh_rate;
		pGameState->targetFrameTime = 1.0 / (double) pGameState->targetFPS;
	}

	pWindow = SDL_CreateWindow(
		"Arcwell Game 2D ver 0.09",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
	);

	if (!pWindow) return -1;

	return 0;
}

/*
 * This method creates a renderer and checks had been the renderer actually created.
 * If the renderer is NULL when this method tried to create it with the graphics card
 * acceleration, then it will try to create it with software renderer, otherwise,
 * method will send an error message and return NULL, that need to be handled in main().
 */
int S_RendererInit()
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); // Avoiding scale blur
	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

	if (pRenderer == NULL)
	{
		pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_SOFTWARE);
		return -1;
	}

	SDL_RenderSetLogicalSize(pRenderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
	return 0;
}

void S_FontInit(const char* filePath, int size)
{
	if (font.file != NULL)
		TTF_CloseFont(font.file);

	font.file = TTF_OpenFont(filePath, size);
}

SDL_Texture* loadTextureFromData(void* textureData, uint32_t currentTextureSize)
{
	SDL_RWops* rw = SDL_RWFromMem(textureData, currentTextureSize);
	SDL_Texture* output = IMG_LoadTexture_RW(pRenderer, rw, 1);

	free(textureData);
	return output;
}

void S_InitTilemapTextureFromData(void* textureData, uint32_t currentTextureSize)
{
	if (!textureData) return;

	tilemap_sprite = loadTextureFromData(textureData, currentTextureSize);
}

void S_InitEntityTextureFromData(void* textureData, uint32_t currentTextureSize, enum ENTITY_ID id)
{
	if (!textureData) return;

	entity_sprites[id] = loadTextureFromData(textureData, currentTextureSize);
}

void S_InitObjTextureFromData(void* textureData, uint32_t currentTextureSize, enum OBJ_ID id)
{
	if (!textureData) return;

	obj_sprites[id] = loadTextureFromData(textureData, currentTextureSize);
}

void S_RenderText(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (text == NULL) return;

	if (font.textTexture != NULL)
	{
		SDL_DestroyTexture(font.textTexture);
		font.textTexture = NULL;
	}

	SDL_Surface* tempSurface = TTF_RenderText_Blended(font.file, text, font.textColor);
	if (tempSurface != NULL)
	{
		font.textTexture = SDL_CreateTextureFromSurface(pRenderer, tempSurface);

		font.textRect.x = x;
		font.textRect.y = y;
		font.textRect.w = tempSurface->w;
		font.textRect.h = tempSurface->h;

		font.textColor.r = r;
		font.textColor.g = g;
		font.textColor.b = b;
		font.textColor.a = a;

		SDL_FreeSurface(tempSurface);
	}

	if (font.textTexture != NULL)
		SDL_RenderCopy(pRenderer, font.textTexture, NULL, &font.textRect);
}

void S_RenderEntity(e_manager_t* pEntManager, int i, int screenX, int screenY, int flip)
{
	SDL_Rect srcRect;
	srcRect.w = ENTITY_SPRITE_SIZE;
	srcRect.h = ENTITY_SPRITE_SIZE;
	srcRect.x = pEntManager->sprites[i].srcX;
	srcRect.y = pEntManager->sprites[i].srcY;

	SDL_Rect destRect;
	destRect.w = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	destRect.h = ENTITY_SPRITE_SIZE * ENTITY_SPRITE_SCALE;
	destRect.x = screenX;
	destRect.y = screenY;

	SDL_RenderCopyEx(pRenderer, entity_sprites[pEntManager->id[i]], &srcRect, &destRect, 0.0, NULL, flip);
}

void S_RenderObject(obj_manager_t* pObjManager, int i, int screenX, int screenY)
{
	SDL_Rect srcRect;
	srcRect.w = TILE_SPRITE_SIZE;
	srcRect.h = TILE_SPRITE_SIZE;
	srcRect.x = pObjManager->sprites[i].srcX;
	srcRect.y = pObjManager->sprites[i].srcY;

	SDL_Rect destRect;
	destRect.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	destRect.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	destRect.x = screenX;
	destRect.y = screenY;

	SDL_RenderCopy(
		pRenderer, obj_sprites[pObjManager->id[i]],
		&srcRect, &destRect
	);
}

void S_RenderLocation(map_t* pLocation, int ix, int iy, int screenX, int screenY)
{
	SDL_Rect srcRect = {
		.w = TILE_SPRITE_SIZE,
		.h = TILE_SPRITE_SIZE,
		.x = pLocation->locationTiles[iy * pLocation->columns + ix].srcX,
		.y = pLocation->locationTiles[iy * pLocation->columns + ix].srcY
	};

	SDL_Rect destRect;
	destRect.w = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	destRect.h = TILE_SPRITE_SIZE * TILE_SPRITE_SCALE;
	destRect.x = screenX;
	destRect.y = screenY;

	SDL_RenderCopy(
		pRenderer, tilemap_sprite,
		&srcRect,
		&destRect
	);
}

/*
 * Destructor method to clean up all renderer textures, close files and quit the SDL
 * (Always need to be called in application crash or normal exit!!!)
 */
void S_Destruct(obj_manager_t* pObjManager, e_manager_t* pEntManager)
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

	if (tilemap_sprite != NULL)
	{
		SDL_DestroyTexture(tilemap_sprite);
		tilemap_sprite = NULL;
	}

	for (int i = 0; i < MAX_SPRITES; ++i)
	{
		if (obj_sprites[i] != NULL)
		{
			SDL_DestroyTexture(obj_sprites[i]);
			obj_sprites[i] = NULL;
		}
	}

	for (int i = 0; i < MAX_SPRITES; ++i)
	{
		if (entity_sprites[i] != NULL)
		{
			SDL_DestroyTexture(entity_sprites[i]);
			entity_sprites[i] = NULL;
		}
	}

	if (pRenderer != NULL)
		SDL_DestroyRenderer(pRenderer);

	if (pWindow != NULL)
		SDL_DestroyWindow(pWindow);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

#endif /* SDL2_X64_WINDOWS */


