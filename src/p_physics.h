#ifndef P_PHYSICS_H_
#define P_PHYSICS_H_

#define FLOOR_DISTANCE 512

typedef struct GameState gamestate_t;
typedef struct EntityManager e_manager_t;
typedef struct Map map_t;
typedef struct Rect rect_t;

extern float 	gravity;
extern float 	jump_force;
extern float 	player_speed;
extern float 	camera_speed;
extern float 	knockback_strength;

void P_EntityFallJump(e_manager_t* pEntManager, gamestate_t* pGameState);
void P_EntityWallCollisionCheck(map_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState);
void P_EntityToEntityCollisionCheck(e_manager_t* pEntManager, gamestate_t* pGameState);
bool P_IntersectRect(const rect_t* a, const rect_t* b, rect_t* result);

#endif /* P_PHYSICS_H_ */
