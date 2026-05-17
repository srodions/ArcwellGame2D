#ifndef S_SYSTEM_H_
#define S_SYSTEM_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct _GameState gamestate_t;
typedef struct _Keymap keymap_t;
typedef struct _Keystates keystates_t;
typedef struct _EntityManager e_manager_t;
typedef struct _ReactionTimer rtimer_t;
typedef struct _ARCF_Header arcf_header_t;
typedef struct _ARCF_Entry arcf_entry_t;
typedef struct _Location location_t;
typedef struct _ObjectManager obj_manager_t;
enum KBD_KEY_STATE;

// I/O
void S_HandleKeyboardInput(enum KBD_KEY_STATE keyState, keymap_t* keyMap, keystates_t* keyStates);
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
void S_FontInit();
void S_ARC_InitTextures(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable);
void S_ObjectSetter(obj_manager_t* pObjManager, const char* jsonFilePath);
// RENDERER
void S_FrameStart(int* frameStart);
void S_FrameEnd(gamestate_t* pGameState, int* frameStart);
void S_RenderDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager);
void S_RenderObject(obj_manager_t* pObjManager, int i, int screenX, int screenY);
void S_RenderLocation(location_t* pLocation, int ix, int iy, int screenX, int screenY);
void S_RenderEntity(e_manager_t* pEntManager, int i, int screenX, int screenY, int flip);
// COLLISION
void S_DestCollisionCheck(e_manager_t* pEntManager, int i, int j, float screenXCenter, float dt);
// CLEAN
void S_Destruct();

#endif /* S_SYSTEM_H_ */
