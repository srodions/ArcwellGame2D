#ifndef G_ENTITY_H_
#define G_ENTITY_H_

#include <stdio.h>
#include "g_constants.h"

typedef struct EntityManager e_manager_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Map map_t;
typedef struct GameState gamestate_t;
typedef struct ReactionTimer rtimer_t;

void G_EntityInit(e_manager_t* pEntManager, int posX, int posY, float speed, float knockback, int hp, int strength, enum ENTITY_ID id);
void G_SkeletonSpawn(e_manager_t* pEntManager, rtimer_t* timer);
void G_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager);
void G_EntityJump(gamestate_t* pGameState, e_manager_t* pEntManager, int index);
void G_SetState(int index, e_manager_t* pEntManager, enum ENTITY_STATE state);
void G_SetAi(int index, e_manager_t* pEntManager, enum ENTITY_AI ai);
void G_MarkEntityToRemove(int index, e_manager_t* pEntManager);
void G_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager);
void G_AI_Chase(gamestate_t* pGameState, e_manager_t* pEntManager);
void G_AI_Idle(e_manager_t* pEntManager);

#endif /* G_ENTITY_H_ */
