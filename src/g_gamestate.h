#ifndef G_GAMESTATE_H_
#define G_GAMESTATE_H_

typedef struct GameState gamestate_t;
typedef struct EntityManager e_manager_t;

gamestate_t G_GameInit();
void G_UpdateDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager);

#endif /* G_GAMESTATE_H_ */
