#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "g_constants.h"

typedef struct GameState
{
	double 			targetFrameTime;
	double 			deltaTime;
	unsigned int 	screenW;
	unsigned int 	screenH;
	int 			targetFPS;
	int 			currentFPS;
	bool 			isRunning;
	bool 			isPaused;
	bool 			isDebugMode;
} gamestate_t;

typedef struct Rect {
    float x, y;
    int w, h;
} rect_t;

typedef struct ARCF_Header
{
	char signature[4]; 			// ARCF
	uint32_t lumpsCount;		// Files count in arc file
	uint32_t offsetToLumpsTable;// Offset to table of files in bytes
} arcf_header_t;

typedef struct ARCF_MapEntryHeader
{
	uint32_t mapRows;
	uint32_t mapColumns;
} arcf_mapheader_t;

typedef struct _ARCF_ObjEntry
{
	uint32_t spriteIndex;
	uint32_t bySpriteX;
	uint32_t bySpriteY;
	uint32_t byTileX;
	uint32_t byTileY;
	bool isAnimated;
} arcf_objentry_t;

typedef struct _ARCF_ObjHeader
{
	uint32_t objCount;
	arcf_objentry_t items[];
} arcf_objheader_t;

typedef struct ARCF_Entry
{
    uint32_t offsetToFile;		// Offset to current file in the table in bytes
    uint32_t lumpSize;			// Size of current file in bytes
    char lumpName[16];			// The name(id) of the file to find it in the table
} arcf_entry_t;

typedef struct Keystates
{
	bool isUp;
	bool isDown;
    bool isLeft;
    bool isRight;
    bool isSpace;
    bool isUse;
    bool isCancel;
    bool isDebug;
    bool isExit;
} keystates_t;

typedef struct EntityCombatParams
{
	float 			knockback;
	int				strength;
	int				hp;
} e_cbt_prms_t;

typedef struct ReactionTimer
{
	uint32_t 		currentTime;
	uint32_t 		lastTime;
	unsigned int 	reactionTime;
} rtimer_t;

typedef struct ObjSprite
{
	int				srcX;
	int				srcY;
	int				currentSprite;
} obj_sprite_t;

typedef struct ObjTransform
{
	float			logX;
	float			logY;
	int				hitboxW;
	int				hitboxH;
} obj_tform_t;

typedef struct ObjectManager
{
	obj_sprite_t	sprites[MAX_OBJECTS];
	rtimer_t		animTimer[MAX_OBJECTS];
	obj_tform_t		transforms[MAX_OBJECTS];
	bool 			isAnimated[MAX_OBJECTS];
	enum OBJ_ID		id[MAX_OBJECTS];
	int				objCount;
} obj_manager_t;

typedef struct Tile
{
	int				srcX;
	int				srcY;
	int				posX;
	int				posY;
} tile_t;

typedef struct Map
{
	tile_t* 		locationTiles;
	uint32_t		rows;
	uint32_t		columns;
} map_t;

typedef struct EntitySprite
{
	int				srcX;
	int				srcY;
	int				srcW;
	int				srcH;
	int				currentSprite;
	enum ENTITY_DIR	direction;
} e_sprite_t;

typedef struct EntityTransform
{
	float 	logX, logY;			// Logical coordinates
	int		hitboxW, hitboxH;
	int		flip;
} e_tform_t;

typedef struct EntityVel
{
	float 	gravityAccel;
	float 	currentSpeed;
} e_vel_t;

typedef struct EntityAI
{
	float	targetDist;
	float 	targetX;
	float	targetY;
	int 	currentChoice;
	bool 	isCollisionOnLeft;
	bool 	isCollisionOnRight;
} e_ai_t;

typedef struct EntityManager
{
	rtimer_t			destructTimer[MAX_ENTITIES];
	rtimer_t			aiTimer[MAX_ENTITIES];
	rtimer_t			animTimer[MAX_ENTITIES];
	e_sprite_t			sprites[MAX_ENTITIES];
	e_tform_t 			transforms[MAX_ENTITIES];
	e_vel_t				velocities[MAX_ENTITIES];
	e_ai_t				aiParams[MAX_ENTITIES];
	e_cbt_prms_t		combatParams[MAX_ENTITIES];
	enum ENTITY_STATE 	state[MAX_ENTITIES];
	enum ENTITY_AI		ai[MAX_ENTITIES];
	enum ENTITY_ID 		id[MAX_ENTITIES];
	bool				isMoving[MAX_ENTITIES];
	bool				isFalling[MAX_ENTITIES];
	int					entitiesCount;
} e_manager_t;

#endif /* TYPEDEFS_H_ */
