#ifndef S_SYSTEM_H_
#define S_SYSTEM_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "g_constants.h"

typedef struct GameState gamestate_t;
typedef struct Keymap keymap_t;
typedef struct ButtonMap btnmap_t;
typedef struct Keystates keystates_t;
typedef struct EntityManager e_manager_t;
typedef struct ReactionTimer rtimer_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Map map_t;
typedef struct ObjectManager obj_manager_t;

// I/O
void S_HandleKeyboardInput(enum KEY_STATE keyState, keymap_t* keyMap, keystates_t* keyStates);
void S_HandleGamepadInput(enum KEY_STATE keyState, btnmap_t* buttonMap, keystates_t* keyStates);
void S_HandleEvents(gamestate_t *pGameState, e_manager_t* pEntManager, keystates_t* keyStates);
void S_ReactionTimerStart(rtimer_t* pReactionTimer);
void S_ReactionTimerEnd(rtimer_t* pReactionTimer);
void S_ReactionTimerReset(rtimer_t* pReactionTimer);
bool S_IsTimeToReact(rtimer_t* pReactionTimer);

// INIT
int S_LibInit();
int S_WindowInit(gamestate_t* pGameState);
int S_RendererInit();
void S_InitKeymap();
void S_InitBtnMap();
void* S_InitGamepad();
void S_FontInit(const char* filePath, int size);
void S_InitTilemapTextureFromData(void* textureData, uint32_t currentTextureSize);
void S_InitObjTextureFromData(void* textureData, uint32_t currentTextureSize, enum OBJ_ID id);
void S_InitEntityTextureFromData(void* textureData, uint32_t currentTextureSize, enum ENTITY_ID id);

// RENDERER
void S_FrameStart(uint64_t* frameStart);
void S_FrameEnd(gamestate_t* pGameState, uint64_t* frameStart);
void S_RenderText(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void S_RenderObject(obj_manager_t* pObjManager, int i, int screenX, int screenY);
void S_RenderLocation(map_t* pLocation, int ix, int iy, int screenX, int screenY);
void S_RenderEntity(e_manager_t* pEntManager, int i, int screenX, int screenY, int flip);

// CLEAN
void S_Destruct();

#endif /* S_SYSTEM_H_ */
