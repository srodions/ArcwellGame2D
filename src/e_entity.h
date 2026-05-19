#ifndef E_ENTITY_H_
#define E_ENTITY_H_

#include <stdio.h>
#include "g_constants.h"

typedef struct EntityManager e_manager_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Map map_t;
typedef struct GameState gamestate_t;

void E_EntitySpritesInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable);
void E_EntityInit(e_manager_t* pEntManager, int posX, int posY, float speed, enum ENTITY_ID id);
void E_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager);
void E_MarkEntityToRemove(int index, e_manager_t* pEntManager);
void E_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager);
void E_AI_Chase(e_manager_t* pEntManager);
void E_AI_Idle(e_manager_t* pEntManager);
void E_Destruct(e_manager_t* pEntManager);

#endif /* E_ENTITY_H_ */
