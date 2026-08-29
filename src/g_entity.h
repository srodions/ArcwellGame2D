#ifndef G_ENTITY_H_
#define G_ENTITY_H_

#include <stdio.h>
#include "typedefs.h"
#include "g_constants.h"
#include "fixed_math.h"

typedef struct EntityManager e_manager_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Map map_t;
typedef struct GameState gamestate_t;
typedef struct ReactionTimer rtimer_t;
typedef struct EntityConfig e_config_t;
typedef struct EntityCfgManager e_cfgmanager_t;

void G_LoadEntityConfigs(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable);
void G_EntityInit(e_manager_t* pEntManager, int atlasSprIdx, int posX, int posY, int speed, int maxHP, int dlf);
void G_UpdateEntity(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager);
void G_EntityRespawn(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager);
void G_EntityDirection(gamestate_t* pGameState, e_manager_t* pEntManager, int i);
void G_EntityJump(gamestate_t* pGameState, e_manager_t* pEntManager, int index);
void G_SetState(int index, e_manager_t* pEntManager, enum ENTITY_STATE state);
void G_SetAi(int index, e_manager_t* pEntManager, enum ENTITY_AI ai);
void G_MarkEntityToRemove(int index, e_manager_t* pEntManager);
void G_RemoveEntityFromLoadList(int index, e_manager_t* pEntManager);
void G_AI_Chase(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int i);
void G_AI_Idle(e_manager_t* pEntManager, int i);
void G_EntityAttack(e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager, int attackerId, int victimId);
void G_EntityHPControl(gamestate_t* pGameState, e_manager_t* pEntManager, int i);

#endif /* G_ENTITY_H_ */
