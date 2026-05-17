#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "s_system.h"
#include "cJSON.h"
#include "h_keyboard.h"
#include "l_location.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "typedefs.h"

typedef struct _Keymap
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

typedef struct _Font
{
	TTF_Font* file;
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;

	SDL_Color textColor;
	SDL_Rect textRect;
} font_t;

// INPUT
SDL_Scancode _input_keyScancode;
keymap_t _keyMap;
// RENDERER
SDL_Window* pWindow;
SDL_Renderer* pRenderer;
font_t font;
// TEXTURES
SDL_Texture* entity_sprites[MAX_ENTITY_SPRITES];
SDL_Texture* obj_sprites[MAX_OBJ_SPRITES];
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
    _keyMap.up = SDL_SCANCODE_W;
    _keyMap.down = SDL_SCANCODE_S;
    _keyMap.left = SDL_SCANCODE_A;
    _keyMap.right = SDL_SCANCODE_D;
    _keyMap.space = SDL_SCANCODE_SPACE;
    _keyMap.use = SDL_SCANCODE_E;
    _keyMap.debug = SDL_SCANCODE_T;
    _keyMap.remove = SDL_SCANCODE_R;
    _keyMap.exit = SDL_SCANCODE_ESCAPE;
}

/*
 * This method handles keyboard input for setting keys' states true or false depending on
 * whether these keys pressed (down) or released (up).
 */
void S_HandleKeyboardInput(enum KBD_KEY_STATE keyState, keymap_t* keyMap, keystates_t* keyStates)
{
	if (_input_keyScancode == keyMap->up)
		keyStates->isUp = keyState;
	else if (_input_keyScancode == keyMap->down)
		keyStates->isDown = keyState;

	if (_input_keyScancode == keyMap->left)
		keyStates->isLeft = keyState;
	else if (_input_keyScancode == keyMap->right)
		keyStates->isRight = keyState;

	if (_input_keyScancode == keyMap->exit)
		keyStates->isExit = keyState;
	if (_input_keyScancode == keyMap->space)
		keyStates->isSpace = keyState;
	if (_input_keyScancode == keyMap->use)
		keyStates->isUse = keyState;
	if (_input_keyScancode == keyMap->debug)
		keyStates->isDebug = keyState;
	if (_input_keyScancode == keyMap->remove)
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
		_input_keyScancode = event.key.keysym.scancode;
		S_HandleKeyboardInput(KEY_STATE_DOWN, &_keyMap, keyStates);
		break;
	case SDL_KEYUP:
		_input_keyScancode = event.key.keysym.scancode;
		S_HandleKeyboardInput(KEY_STATE_UP, &_keyMap, keyStates);
		break;
	}
}

void S_DestCollisionCheck(e_manager_t* pEntManager, int i, int j, float screenXCenter, float dt)
{
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

void S_FontInit()
{
	font.file = TTF_OpenFont("res/font/x12y16pxMaruMonica.ttf", 40);
	font.textColor.r = 255;
	font.textColor.g = 255;
	font.textColor.b = 255;
	font.textColor.a = 255;
}

void S_ARC_InitTextures(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable)
{
	uint32_t currentTextureSize = 0;

	void* tileMapTextureData = L_LoadLump(arcFile, "TILES", pHeader, pTable, &currentTextureSize);
	if (tileMapTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(tileMapTextureData, currentTextureSize);
		tilemap_sprite = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(tileMapTextureData);
	}

	void* playerTextureData = L_LoadLump(arcFile, "PLAYER", pHeader, pTable, &currentTextureSize);
	if (playerTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(playerTextureData, currentTextureSize);
		entity_sprites[0] = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(playerTextureData);
	}

	void* skeletonTextureData = L_LoadLump(arcFile, "SKELETON", pHeader, pTable, &currentTextureSize);
	if (skeletonTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(skeletonTextureData, currentTextureSize);
		entity_sprites[1] = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(skeletonTextureData);
	}

	void* torchTextureData = L_LoadLump(arcFile, "TORCH", pHeader, pTable, &currentTextureSize);
	if (torchTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(torchTextureData, currentTextureSize);
		obj_sprites[0] = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(torchTextureData);
	}

	void* decorationTextureData = L_LoadLump(arcFile, "DECORATION", pHeader, pTable, &currentTextureSize);
	if (decorationTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(decorationTextureData, currentTextureSize);
		obj_sprites[1] = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(decorationTextureData);
	}

	void* chestTextureData = L_LoadLump(arcFile, "CHEST", pHeader, pTable, &currentTextureSize);
	if (chestTextureData)
	{
		SDL_RWops* rw = SDL_RWFromMem(chestTextureData, currentTextureSize);
		obj_sprites[2] = IMG_LoadTexture_RW(pRenderer, rw, 1);
		free(chestTextureData);
	}
}

void S_ObjectSetter(obj_manager_t* pObjManager, const char* jsonFilePath)
{
	char* data = (char*) SDL_LoadFile(jsonFilePath, NULL);

	cJSON *json = cJSON_Parse(data);

	if (json == NULL)
	{
		const char* errorMsg = cJSON_GetErrorPtr();
		if (errorMsg != NULL)
			fprintf(stderr, "JSON Parse Error: %s\n", errorMsg);
		SDL_free(data);
		return;
	}

	cJSON* objects = cJSON_GetObjectItem(json, "objects");
	cJSON* object = NULL;

	cJSON_ArrayForEach(object, objects)
	{
		int spriteIndex = cJSON_GetObjectItem(object, "spriteIndex")->valueint;
		int bsx = cJSON_GetObjectItem(object, "bySpriteX")->valueint;
		int bsy = cJSON_GetObjectItem(object, "bySpriteY")->valueint;
	    int btx = cJSON_GetObjectItem(object, "byTileX")->valueint;
	    int bty = cJSON_GetObjectItem(object, "byTileY")->valueint;
	    bool isAnim = cJSON_IsTrue(cJSON_GetObjectItem(object, "isAnimated"));

	    L_ObjectInit(pObjManager, spriteIndex, bsx, bsy, btx, bty, isAnim);
	}

	if (data != NULL)
		SDL_free(data);
}

void S_RenderDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (font.textTexture != NULL)
	{
		SDL_DestroyTexture(font.textTexture);
		font.textTexture = NULL;
	}

	char textBuffer[64];
	snprintf(
		textBuffer, sizeof(textBuffer), "FPS: %d | Entities: %d | x: %d | y: %d",
		pGameState->currentFPS, pEntManager->entitiesCount,
		(int) pEntManager->transforms[0].logX, (int) pEntManager->transforms[0].logY
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

void S_RenderLocation(location_t* pLocation, int ix, int iy, int screenX, int screenY)
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

	for (int i = 0; i < MAX_OBJ_SPRITES; ++i)
	{
		if (obj_sprites[i] != NULL)
		{
			SDL_DestroyTexture(obj_sprites[i]);
			obj_sprites[i] = NULL;
		}
	}

	for (int i = 0; i < MAX_ENTITY_SPRITES; ++i)
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


