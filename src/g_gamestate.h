#ifndef G_GAMESTATE_H_
#define G_GAMESTATE_H_

typedef struct GameState gamestate_t;
typedef struct EntityManager e_manager_t;
typedef struct EntityCfgManager e_cfgmanager_t;

gamestate_t G_GameStateInit();
void G_UpdateDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager);
void G_GameRestart(gamestate_t* pGameState, e_manager_t* pEntManager, e_cfgmanager_t* pEntCfgManager);

#endif /* G_GAMESTATE_H_ */
