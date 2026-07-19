#ifndef I_SYSTEM_H_
#define I_SYSTEM_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "g_constants.h"
#include "fixed_math.h"

typedef struct GameState gamestate_t;
typedef struct Keymap keymap_t;
typedef struct ButtonMap btnmap_t;
typedef struct EntityManager e_manager_t;
typedef struct ReactionTimer rtimer_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Map map_t;
typedef struct ObjectManager obj_manager_t;
typedef struct InputState inputstate_t;
typedef enum InputMask inputmask_t;

extern keymap_t keyMap;
extern btnmap_t btnMap;

// I/O
void I_HandleGamepadAxis(uint16_t* nextFrameInput);
void I_HandleEvents(gamestate_t *pGameState, e_manager_t* pEntManager, inputstate_t* input);
void I_ReactionTimerStart(rtimer_t* pReactionTimer);
void I_ReactionTimerEnd(rtimer_t* pReactionTimer);
void I_ReactionTimerReset(rtimer_t* pReactionTimer);
inputmask_t I_KeyMapToTrigger(int scancode);
inputmask_t I_GamepadMapToTrigger(uint8_t scancode);
bool I_IsTimeToReact(rtimer_t* pReactionTimer);

// INIT
int I_LibInit();
int I_WindowInit(gamestate_t* pGameState);
int I_RendererInit();
void I_InitKeymap();
void I_InitBtnMap();
void I_LoadFont(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, int size);

// RENDERER
void I_FrameStart(uint64_t* frameStart);
void I_FrameEnd(gamestate_t* pGameState, uint64_t* frameStart);
void I_RenderText(gamestate_t* pGameState, const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void I_RenderScene(map_t* pLocation, obj_manager_t* pObjManager, e_manager_t* pEntManager);

// CLEAN
void I_Destruct();

#endif /* I_SYSTEM_H_ */
