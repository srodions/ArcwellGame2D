#ifndef P_PHYSICS_H_
#define P_PHYSICS_H_

#define FLOOR_DISTANCE 512

typedef struct _GameState gamestate_t;
typedef struct _EntityManager e_manager_t;
typedef struct _Location location_t;

extern float 	gravity;
extern float 	jump_force;
extern float 	player_speed;
extern float 	camera_speed;
extern float 	knockback_strength;

void P_EntityFallJump(e_manager_t* pEntManager, gamestate_t* pGameState);
void P_EntityWallCollisionCheck(location_t* pLocation, e_manager_t* pEntManager, gamestate_t* pGameState);
void P_EntityToEntityCollisionCheck(e_manager_t* pEntManager, gamestate_t* pGameState);

#endif /* P_PHYSICS_H_ */
