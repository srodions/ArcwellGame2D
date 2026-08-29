
#if defined(SDL2_X64)

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include "i_system.h"
#include "r_renderer.h"
#include "g_map.h"
#include "h_input.h"
#include "p_physics.h"
#include "l_arcloader.h"
#include "typedefs.h"

// KEYBOARD
typedef struct Keymap
{
    SDL_Scancode up;
    SDL_Scancode left;
    SDL_Scancode down;
    SDL_Scancode right;
    SDL_Scancode w;
	SDL_Scancode a;
	SDL_Scancode s;
	SDL_Scancode d;
    SDL_Scancode i;
    SDL_Scancode j;
    SDL_Scancode k;
    SDL_Scancode l;
    SDL_Scancode space;
    SDL_Scancode esc;
    SDL_Scancode f3;
} keymap_t;

// JOYSTICK
typedef struct ButtonMap
{
	Uint8 up;
	Uint8 left;
	Uint8 down;
	Uint8 right;
	Uint8 y;
	Uint8 x;
	Uint8 a;
	Uint8 b;
	Uint8 back;
	Uint8 start;
} btnmap_t;

// INPUT KEYBOARD
keymap_t keyMap;
// INPUT GAMEPAD
static SDL_GameController* gamepad = NULL;
btnmap_t btnMap;
// RENDERER
static SDL_Window* pWindow = NULL;
static SDL_Renderer* pRenderer = NULL;
// TEXTURES
static SDL_Texture* streaming_texture = NULL;

/*
 * Starts the timer from current number of milliseconds since SDL library initialization.
 */
void I_ReactionTimerStart(rtimer_t* pReactionTimer)
{
	pReactionTimer->currentTime = SDL_GetTicks();
}

void I_ReactionTimerEnd(rtimer_t* pReactionTimer)
{
	pReactionTimer->lastTime = pReactionTimer->currentTime;
}

void I_ReactionTimerReset(rtimer_t* pReactionTimer)
{
    pReactionTimer->lastTime = SDL_GetTicks();
}

/*
 * Checks whether is delta time reached reaction time.
 */
bool I_IsTimeToReact(rtimer_t* pReactionTimer)
{
	return pReactionTimer->currentTime - pReactionTimer->lastTime >= pReactionTimer->reactionTime;
}

/*
 * This method initializes key map setup for controls.
 */
void I_InitKeymap()
{
    keyMap.up 		= SDL_SCANCODE_UP;
    keyMap.left 	= SDL_SCANCODE_LEFT;
    keyMap.down 	= SDL_SCANCODE_DOWN;
    keyMap.right 	= SDL_SCANCODE_RIGHT;
    keyMap.w		= SDL_SCANCODE_W;
	keyMap.a		= SDL_SCANCODE_A;
	keyMap.s		= SDL_SCANCODE_S;
	keyMap.d		= SDL_SCANCODE_D;
	keyMap.i		= SDL_SCANCODE_I;
	keyMap.j		= SDL_SCANCODE_J;
	keyMap.k		= SDL_SCANCODE_K;
	keyMap.l		= SDL_SCANCODE_L;
	keyMap.space 	= SDL_SCANCODE_SPACE;
    keyMap.esc 		= SDL_SCANCODE_ESCAPE;
    keyMap.f3		= SDL_SCANCODE_F3;
}

void I_InitBtnMap()
{
	btnMap.up 		= SDL_CONTROLLER_BUTTON_DPAD_UP;
	btnMap.left 	= SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	btnMap.down 	= SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	btnMap.right 	= SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	btnMap.y 		= SDL_CONTROLLER_BUTTON_Y;
	btnMap.x 		= SDL_CONTROLLER_BUTTON_X;
	btnMap.a 		= SDL_CONTROLLER_BUTTON_A;
	btnMap.b 		= SDL_CONTROLLER_BUTTON_B;
	btnMap.back 	= SDL_CONTROLLER_BUTTON_BACK;
	btnMap.start	= SDL_CONTROLLER_BUTTON_START;
}

inputmask_t I_KeyMapToTrigger(int scancode)
{
	if (scancode == keyMap.up    || scancode == keyMap.w)     return INPUT_UP;
	if (scancode == keyMap.left  || scancode == keyMap.a)     return INPUT_LEFT;
	if (scancode == keyMap.down  || scancode == keyMap.s)     return INPUT_DOWN;
	if (scancode == keyMap.right || scancode == keyMap.d)     return INPUT_RIGHT;

	if (scancode == keyMap.i)     return INPUT_SPEC_ATK;
	if (scancode == keyMap.j)     return INPUT_USE;
	if (scancode == keyMap.k)     return INPUT_JUMP;
	if (scancode == keyMap.l)     return INPUT_ATTACK;

	if (scancode == keyMap.esc)   return INPUT_EXIT;
	if (scancode == keyMap.space) return INPUT_PAUSE;
	if (scancode == keyMap.f3)    return INPUT_DEBUG;

	return 0;
}

inputmask_t I_GamepadMapToTrigger(uint8_t scancode)
{
	if (scancode == btnMap.up)		return INPUT_UP;
	if (scancode == btnMap.left)	return INPUT_LEFT;
	if (scancode == btnMap.down)	return INPUT_DOWN;
	if (scancode == btnMap.right)	return INPUT_RIGHT;

	if (scancode == btnMap.y)     	return INPUT_SPEC_ATK;
	if (scancode == btnMap.x)     	return INPUT_USE;
	if (scancode == btnMap.a)     	return INPUT_JUMP;
	if (scancode == btnMap.b)     	return INPUT_ATTACK;

	if (scancode == btnMap.back)   	return INPUT_EXIT;
	if (scancode == btnMap.start) 	return INPUT_PAUSE;

	return 0;
}

void I_HandleGamepadAxis(uint16_t* nextFrameInput)
{
	if (!gamepad || !SDL_GameControllerGetAttached(gamepad)) return;

	// Raw left stick axis
	Sint16 rawX = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
	Sint16 rawY = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);

	// Horizontal axis (left/right)
	if (rawX < -STICK_DEADZONE)
	{
		*nextFrameInput |=  INPUT_LEFT;   // Left
		*nextFrameInput &= ~INPUT_RIGHT;
	}
	else if (rawX > STICK_DEADZONE)
	{
		*nextFrameInput |=  INPUT_RIGHT;  // Right
		*nextFrameInput &= ~INPUT_LEFT;
	}
	else
	{
		// Stick is in the deadzone, both directions are false
		*nextFrameInput &= ~INPUT_LEFT;
		*nextFrameInput &= ~INPUT_RIGHT;
	}

	// Vertical axis (up/down)
	if (rawY < -STICK_DEADZONE)
	{
		*nextFrameInput |=  INPUT_UP;
		*nextFrameInput &= ~INPUT_DOWN;
	}
	else if (rawY > STICK_DEADZONE)
	{
		*nextFrameInput |=  INPUT_DOWN;
		*nextFrameInput &= ~INPUT_UP;
	}
	else
	{
		// Stick is in the deadzone, both directions are false
		*nextFrameInput &= ~INPUT_UP;
		*nextFrameInput &= ~INPUT_DOWN;
	}
}

/*
 * This function handles events that come to the SDL window and stay in queue.
 * These events can be: application's quit button event, any key pressed event,
 * any key released event, etc.
 */
void I_HandleEvents(gamestate_t* pGameState, e_manager_t* pEntManager, inputstate_t* input)
{
	SDL_Event event;
	input->previous = input->current;
	uint16_t nextFrameInput = input->current;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		// WINDOW EVENT
		case SDL_QUIT:
			pGameState->isRunning = false;
			break;
		// KEYBOARD HANDLING
		case SDL_KEYDOWN:
			if (event.key.repeat == 0)
				nextFrameInput |= I_KeyMapToTrigger(event.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			nextFrameInput &= ~I_KeyMapToTrigger(event.key.keysym.scancode);
			break;
		// GAMEPAD HANDLING
		case SDL_CONTROLLERDEVICEADDED:
		    if (!gamepad)
		    {
		        gamepad = SDL_GameControllerOpen(event.cdevice.which);
		        if (gamepad)
		        {
		        	printf("[INPUT]::(LOG) Controller '%s' connected\n", SDL_GameControllerName(gamepad));
		        	pGameState->isGamepadAttached = true;
		        }
		    }
		    break;

		case SDL_CONTROLLERDEVICEREMOVED:
		    if (gamepad && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamepad)))
		    {
		    	printf("[INPUT]::(LOG) Controller '%s' disconnected\n", SDL_GameControllerName(gamepad));
		        SDL_GameControllerClose(gamepad);
		        gamepad = NULL;
		        pGameState->isGamepadAttached = false;
		    }
		    break;
		case SDL_CONTROLLERBUTTONDOWN:
			nextFrameInput |= I_GamepadMapToTrigger(event.cbutton.button);
			break;
		case SDL_CONTROLLERBUTTONUP:
			nextFrameInput &= ~I_GamepadMapToTrigger(event.cbutton.button);
			break;
		case SDL_CONTROLLERAXISMOTION:
			I_HandleGamepadAxis(&nextFrameInput);
			break;
		}
	}

	input->current = nextFrameInput;
}

void I_FrameStart(uint64_t* frameStart)
{
    *frameStart = SDL_GetPerformanceCounter();
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);
}

void I_FrameEnd(gamestate_t* pGameState, uint64_t* frameStart)
{
	SDL_UpdateTexture(streaming_texture, NULL, r_screenBuffer, SCR_LOGICAL_WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(pRenderer, streaming_texture, NULL, NULL);
	SDL_RenderPresent(pRenderer);

	uint64_t frameEnd = SDL_GetPerformanceCounter();
	uint64_t elapsedCounters = frameEnd - *frameStart;
	uint64_t freq = SDL_GetPerformanceFrequency();

	fixed_t counterElapsed = (fixed_t)((elapsedCounters << FIXED_SHIFT) / freq);

	if (counterElapsed < pGameState->targetFrameTime)
	{
		fixed_t diff = pGameState->targetFrameTime - counterElapsed;
		uint32_t delayMs = (uint32_t)((diff * 1000) >> FIXED_SHIFT);
		if (delayMs > 0) SDL_Delay(delayMs);

		pGameState->deltaTime = pGameState->targetFrameTime;
	}
	else
	{
		pGameState->deltaTime = counterElapsed;
	}
}

int I_LibInit()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0 || IMG_Init(IMG_INIT_PNG) < 0)
	{
		printf("[SDL_INIT]::(ERR) Error initializing SDL2 libraries: '%s'\n", SDL_GetError());
		return -1;
	}

	return 0;
}

/*
 * This method creates an application window of the game based on the display settings.
 * It takes display refresh rate for target FPS variable
 */
int I_WindowInit(gamestate_t* pGameState)
{
	pWindow = SDL_CreateWindow(
		"Arcwell Game 2D | Alpha Build ver.0.1.1",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCR_LOGICAL_WIDTH, SCR_LOGICAL_HEIGHT,
		SDL_WINDOW_FULLSCREEN_DESKTOP
	);

	if (!pWindow)
	{
		printf("[WINDOW_INIT]::(ERR) Error creating SDL2 window: '%s'\n", SDL_GetError());
		return -1;
	}

	SDL_DisplayMode dMode;
	int currentDisplayIndex = SDL_GetWindowDisplayIndex(pWindow);

	if (SDL_GetDesktopDisplayMode(currentDisplayIndex, &dMode) == 0)
	{
		// If there is an error fetching refresh rate, then target FPS = 60
		pGameState->targetFPS = dMode.refresh_rate > 0 ? dMode.refresh_rate : 60;
	}
	else
	{
		// Or if there is an error fetching screen mode, then target FPS is also 60
		pGameState->targetFPS = 60;
		printf("[DISPLAY]::(WRN) Error getting display mode: '%s'; targetFPS = 60\n", SDL_GetError());
	}

	pGameState->targetFrameTime = FIXED_ONE / pGameState->targetFPS;

	return 0;
}

/*
 * This method creates a renderer and checks had been the renderer actually created.
 * If the renderer is NULL when this method tried to create it with the graphics card
 * acceleration, then it will try to create it with software renderer, otherwise,
 * method will send an error message and return -1. If streaming texture for screen
 * buffer was not created, then method will return -1. If everything was created properly,
 * then return 0.
 */
int I_RendererInit()
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); // Avoiding scale blur
	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// If accelerated renderer wasn't created - try to create software renderer
	if (pRenderer == NULL)
	{
		printf("[RENDERER]::(WRN) Error creating GPU accelerated renderer: '%s'\n", SDL_GetError());
		pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);

		if (pRenderer == NULL)
		{
			printf("[RENDERER]::(ERR) Error creating software renderer: '%s'\n", SDL_GetError());
			return -1;
		}
		else printf("[RENDERER]::(LOG) Software renderer created\n");
	}
	else printf("[RENDERER]::(LOG) GPU accelerated renderer created\n");

	SDL_RenderSetIntegerScale(pRenderer, SDL_TRUE);
	SDL_RenderSetLogicalSize(pRenderer, SCR_LOGICAL_WIDTH, SCR_LOGICAL_HEIGHT);

	streaming_texture = SDL_CreateTexture(
		pRenderer,
		SDL_PIXELFORMAT_ABGR8888, 		// 4 bytes on pixel (A,B,G,R)
		SDL_TEXTUREACCESS_STREAMING,
		SCR_LOGICAL_WIDTH,
		SCR_LOGICAL_HEIGHT
	);

	if (!streaming_texture)
	{
		printf("[RENDERER]::(ERR) Error creating streaming texture: '%s'\n", SDL_GetError());
		return -1;
	}

	return 0;
}

/*
 * Destructor method to clean up all renderer textures, close files and quit the SDL
 * (!!!Always need to be called in application crash or normal exit!!!)
 */
void I_SDL_Destruct()
{
	if (gamepad)
		SDL_GameControllerClose(gamepad);

	if (streaming_texture != NULL)
	{
		SDL_DestroyTexture(streaming_texture);
		streaming_texture = NULL;
	}

	if (pRenderer != NULL)
		SDL_DestroyRenderer(pRenderer);

	if (pWindow != NULL)
		SDL_DestroyWindow(pWindow);

	IMG_Quit();
	SDL_Quit();
}

#endif /* SDL2_X64 */


