#ifndef R_RENDERER_H_
#define R_RENDERER_H_

typedef struct GameState gamestate_t;
typedef struct Map map_t;
typedef struct EntityManager e_manager_t;
typedef struct ObjectManager obj_manager_t;

void R_RenderLocation(map_t* pLocation, e_manager_t* pEntManager);
void R_RenderObject(obj_manager_t* pObjManager, e_manager_t* pEntManager);
void R_RenderEntity(e_manager_t* pEntManager);
void R_Anim_Spawn(e_manager_t* pEntManager, int i);
void R_Anim_Walk(e_manager_t* pEntManager, int i);
void R_RenderDebugStats(gamestate_t* pGameState, e_manager_t* pEntManager);

#endif /* R_RENDERER_H_ */
