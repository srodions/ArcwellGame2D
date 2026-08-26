#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "fixed_math.h"
#include "g_constants.h"

typedef struct GameState
{
	char 			debugText[DEBUG_TEXT_LENGTH];
	fixed_t 		targetFrameTime;
	fixed_t 		deltaTime;
	fixed_t			fpsTimer;
	int 			targetFPS;
	int 			currentFPS;
	bool 			isRunning;
	bool 			isPaused;
	bool 			isDebugMode;
} gamestate_t;

typedef struct Rect
{
    int x, y;
    int w, h;
} rect_t;

typedef enum InputMask
{
	INPUT_UP	 	= (1 << 0),
	INPUT_LEFT	 	= (1 << 1),
	INPUT_DOWN	 	= (1 << 2),
	INPUT_RIGHT  	= (1 << 3),
	INPUT_SPEC_ATK 	= (1 << 4),
	INPUT_USE	 	= (1 << 5),
	INPUT_JUMP 	 	= (1 << 6),
	INPUT_ATTACK 	= (1 << 7),
	INPUT_PAUSE		= (1 << 8),
	INPUT_EXIT		= (1 << 9),
	INPUT_DEBUG		= (1 << 10)
} inputmask_t;

typedef struct InputState
{
    uint16_t current;
    uint16_t previous;
} inputstate_t;

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
	uint32_t tileAtlasIdx;
	char	data[];
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
    char 	lumpName[16];		// The name(id) of the file to find it in the table
} arcf_entry_t;

typedef struct ARCF_SpriteHeader
{
	uint32_t spriteW;
	uint32_t spriteH;
} arcf_spriteheader_t;

typedef struct ARCF_NamesEntry
{
	uint32_t objCount;		// Amount of objects in file
	uint32_t entCount;		// ...
	uint32_t mapCount;		// ....
	uint32_t uiCount;		// .....
	char	objNames[256];	// Lines with names
	char	entNames[256];	// ...
	char	mapNames[256];	// ....
	char	uiNames[256];	// .....
} arcf_namesentry_t;

typedef struct RenderAsset
{
    void* rawData;
    arcf_spriteheader_t* header;
    uint32_t* pixels;
} renderasset_t;

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
	fixed_t			logX;
	fixed_t			logY;
	int				hitboxW;
	int				hitboxH;
} obj_tform_t;

typedef struct ObjectManager
{
	obj_sprite_t*	sprites;
	rtimer_t*		animTimer;
	obj_tform_t*	transforms;
	bool*			isAnimated;
	int*			sprIndex;
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
	uint32_t		tileAtlasIdx;
} map_t;

typedef struct MapManager
{
	map_t*			maps;
	int 			mapsCount;
} map_manager_t;

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
	fixed_t logX, logY;			// Logical coordinates
	int		hitboxW, hitboxH;
	int		flip;
} e_tform_t;

typedef struct EntityVel
{
	fixed_t 	gravityAccel;
	fixed_t 	currentSpeed;
} e_vel_t;

typedef struct EntityAI
{
	fixed_t	targetDist;
	fixed_t targetX;
	fixed_t	targetY;
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
	enum ENTITY_STATE 	state[MAX_ENTITIES];
	enum ENTITY_AI		ai[MAX_ENTITIES];
	int 				atlasSprIdx[MAX_ENTITIES];
	unsigned int		hp[MAX_ENTITIES];
	int					rdLastDeathSprites[MAX_ENTITIES];
	bool				isMoving[MAX_ENTITIES];
	bool				isFalling[MAX_ENTITIES];
	bool				hasDamaged[MAX_ENTITIES];
	int					entitiesCount;
} e_manager_t;

typedef struct EntityConfig
{
	uint32_t posX;
	uint32_t posY;
	uint32_t atlasSprIdx;
	// FRAME COUNTS
	uint32_t spawnFramesCount;
	uint32_t deathFramesCount;
	uint32_t deathLastFrames;
	uint32_t walkFramesCount;
	uint32_t stayFramesCount;
	uint32_t angerFramesCount;
	uint32_t attackFramesCount;
	// COMBAT
	fixed_t  knockback;
	fixed_t	 speed;
	uint32_t strength;
	uint32_t maxHp;
	uint32_t attackDist;
	uint32_t dmgSpriteIndex; // Index of the sprite on which damage will be taken by the victim
	// ROWS
	uint32_t spawnFramesRow;
	uint32_t deathFramesRow;
	uint32_t angerFramesRow;
	uint32_t attackFramesRow;
	uint32_t walkFramesRow;
	uint32_t stayFramesRow;
} e_config_t;

typedef struct _ARCF_EntCfgHeader
{
	uint32_t cfgCount;
	e_config_t items[];
} arcf_entcfgheader_t;

typedef struct EntityCfgManager
{
	e_config_t* configs;
	int 		cfgCount;
	int			capacity;
} e_cfgmanager_t;

#endif /* TYPEDEFS_H_ */
