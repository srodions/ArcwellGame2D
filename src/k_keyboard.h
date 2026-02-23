#ifndef K_KEYBOARD_H_
#define K_KEYBOARD_H_

#include "g_gamestate.h"
#include "e_entity.h"
#include "p_physics.h"

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

typedef struct _Keystates
{
	bool isUp;
	bool isDown;
    bool isLeft;
    bool isRight;
    bool isSpace;
    bool isUse;
    bool isDebug;
    bool isRemove;
    bool isExit;
} keystates_t;

enum KBD_KEY_STATE
{
    KEY_STATE_UP,
    KEY_STATE_DOWN
};

/* --- DEFINITIONS --- */
void K_InitKeymap();
void K_HandleKeyboardInput(SDL_Scancode keyScancode, enum KBD_KEY_STATE keyState);
void K_HandleEvents(SDL_Renderer* pRenderer, gamestate_t* pGameState, e_manager_t* pEntManager);
void K_HandleKeyStates(SDL_Renderer* pRenderer, gamestate_t* pGameState, e_manager_t* pEntManager);

/* --- IMPLEMENTATIONS --- */
#if defined(STB_KEYBOARD_HANDLER_IMPLEMENTATION)

keymap_t keyMap;
keystates_t keyStates;

/*
 * This method initializes key map setup for controls.
 */
void K_InitKeymap()
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

    keyStates.isUp = false;
    keyStates.isDown = false;
    keyStates.isLeft = false;
    keyStates.isRight = false;
    keyStates.isSpace = false;
    keyStates.isUse = false;
    keyStates.isDebug = false;
    keyStates.isRemove = false;
    keyStates.isExit = false;
}

/*
 * This method handles keyboard input for setting keys' states true or false depending on
 * whether these keys pressed (down) or released (up).
 */
void K_HandleKeyboardInput(SDL_Scancode keyScancode, enum KBD_KEY_STATE keyState)
{
	if (keyScancode == keyMap.up)
		keyStates.isUp = keyState;
	else if (keyScancode == keyMap.down)
		keyStates.isDown = keyState;

	if (keyScancode == keyMap.left)
		keyStates.isLeft = keyState;
	else if (keyScancode == keyMap.right)
		keyStates.isRight = keyState;

	if (keyScancode == keyMap.exit)
		keyStates.isExit = keyState;
	if (keyScancode == keyMap.space)
		keyStates.isSpace = keyState;
	if (keyScancode == keyMap.use)
		keyStates.isUse = keyState;
	if (keyScancode == keyMap.debug)
		keyStates.isDebug = keyState;
	if (keyScancode == keyMap.remove)
		keyStates.isRemove = keyState;
}

/*
 * This function process keys' states boolean (true - key is pressed, false - key is released).
 * If key pressed is true then do a thing, if false - stop it.
 */
void K_HandleKeyStates(SDL_Renderer* pRenderer, gamestate_t* pGameState, e_manager_t* pEntManager)
{
	if (keyStates.isExit)
		pGameState->isRunning = false;

	pEntManager->isMoving[0] = keyStates.isLeft || keyStates.isRight;

	if (keyStates.isUp || keyStates.isSpace)
	{
		if (!pEntManager->isFalling[0])
		{
			pEntManager->velocities[0].gravityAccel = jump_force;
			pEntManager->isFalling[0] = true;
		}
	}

	if (keyStates.isLeft)
		pEntManager->sprites[0].direction = 'L';
	else if (keyStates.isRight)
		pEntManager->sprites[0].direction = 'R';

	if (keyStates.isDebug)
	{
		pGameState->isDebugMode = !pGameState->isDebugMode;
		keyStates.isDebug = false;
	}

	if (keyStates.isUse && pEntManager->entitiesCount < MAX_ENTITIES)
	{
		E_EntityInit(pEntManager, 1, 960, FLOOR_DISTANCE);
		pEntManager->isIdle[pEntManager->entitiesCount - 1] = true;
		keyStates.isUse = false;
	}
	else if (keyStates.isRemove)
	{
		E_RemoveEntityFromLoadList(pEntManager->entitiesCount - 1, pEntManager);
		keyStates.isRemove = false;
	}
}
#endif /* STB_KEYBOARD_HANDLER_IMPLEMENTATION */

#if defined(STB_EVENT_HANDLER_IMPLEMENTATION)
/*
 * This function handles events that come to the SDL window and stay in queue.
 * These events can be: application's quit button event, any key pressed event,
 * any key released event, etc.
 */
void K_HandleEvents(SDL_Renderer* pRenderer, gamestate_t *pGameState, e_manager_t* pEntManager)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		pGameState->isRunning = false;
		break;
	case SDL_KEYDOWN:
		K_HandleKeyboardInput(event.key.keysym.scancode, KEY_STATE_DOWN);
		break;
	case SDL_KEYUP:
		K_HandleKeyboardInput(event.key.keysym.scancode, KEY_STATE_UP);
		break;
	}

	K_HandleKeyStates(pRenderer, pGameState, pEntManager);
}
#endif /* STB_EVENT_HANDLER_IMPLEMENTATION */

#endif /* K_KEYBOARD_H_ */
