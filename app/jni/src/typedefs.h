#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

// LIMITS
#define MAX_ENTITY_SPRITES 	2
#define MAX_OBJ_SPRITES		3
#define MAX_ENTITIES 		32
#define MAX_OBJECTS 		32
#define MAX_MAP_COLUMNS 	12
#define MAX_MAP_ROWS 		64
#define MAX_LOCATIONS 		1
// SPRITES & ANIMATION
#define ENTITY_FRAMES_COUNT 8
#define OBJ_FRAMES_COUNT 	5
#define ANIM_TIME 			200
#define TILE_SPRITE_SIZE 	16
#define TILE_SPRITE_SCALE 	6
#define ENTITY_SPRITE_SIZE 	32
#define ENTITY_SPRITE_SCALE 5
// AI
#define AI_IDLE_MIN_RENEW_TIME 	800
#define AI_IDLE_MAX_RENEW_TIME 	1500
#define AI_CHASE_MIN_RENEW_TIME 400
#define AI_CHASE_MAX_RENEW_TIME 800
// SCREEN
#define LOGICAL_WIDTH 	1920
#define LOGICAL_HEIGHT 	1080
// ETC
#define FLOOR_DISTANCE 512

typedef struct _GameState
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

typedef struct _Keymap
{
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode space;
    SDL_Scancode use;
    SDL_Scancode debug;
    SDL_Scancode remove;
    SDL_Scancode exit;
} keymap_t;

typedef struct _Keystates
{
	bool isUp;
	bool isDown;
    bool isLeft;
    bool isRight;
    bool isSpace;
    bool isUse;
    bool isDebug;
    bool isRemove;
    bool isExit;
} keystates_t;

enum KBD_KEY_STATE
{
    KEY_STATE_UP,
    KEY_STATE_DOWN
};

enum ENTITY_DIR
{
	LEFT,
	RIGHT
};

typedef struct _ReactionTimer
{
	Uint32 			currentTime;
	Uint32 			lastTime;
	unsigned int 	reactionTime;
} rtimer_t;

typedef struct _EntitySprite
{
	SDL_Texture* 		spriteImg;
	SDL_Rect 			spriteSrc;
	SDL_RendererFlip	flip;
	int					currentSprite;
	enum ENTITY_DIR		direction;
} e_sprite_t;

typedef struct _ObjSprite
{
	SDL_Texture* 	spriteImg;
	SDL_Rect 		spriteSrc;
	int				currentSprite;
} obj_sprite_t;

typedef struct _EntityTransform
{
	float 	logX, logY;			// Logical coordinates
	int		hitboxW, hitboxH;
} e_tform_t;

typedef struct _EntityVel
{
	float 	gravityAccel;
} e_vel_t;

typedef struct _EntityAI
{
	float	targetDist;
	float 	targetX;
	float	targetY;
	int 	currentChoice;
	bool 	isCollisionOnLeft;
	bool 	isCollisionOnRight;
} e_ai_t;

typedef struct _EntityManager
{
	rtimer_t	aiTimer[MAX_ENTITIES];
	rtimer_t	animTimer[MAX_ENTITIES];
	e_sprite_t	sprites[MAX_ENTITIES];
	e_tform_t 	transforms[MAX_ENTITIES];
	e_vel_t		velocities[MAX_ENTITIES];
	e_ai_t		aiParams[MAX_ENTITIES];
	bool		isIdle[MAX_ENTITIES];
	bool		isChasing[MAX_ENTITIES];
	bool		isMoving[MAX_ENTITIES];
	bool		isFalling[MAX_ENTITIES];
	SDL_Rect 	entityDest;
	int			entitiesCount;
} e_manager_t;

typedef struct _Tile
{
	SDL_Rect 	tileSrc;
	int			posX, posY;
} tile_t;

typedef struct _ObjectManager
{
	obj_sprite_t	sprites[MAX_OBJECTS];
	rtimer_t		animTimer[MAX_OBJECTS];
	e_tform_t		transforms[MAX_OBJECTS];
	bool 			isAnimated[MAX_OBJECTS];
	SDL_Rect		objDest;
	int				objCount;
} obj_manager_t;

typedef struct _Location
{
	SDL_Texture* 	tileMap;
	SDL_Rect		locationDest;
	tile_t 			locationTiles[MAX_MAP_COLUMNS][MAX_MAP_ROWS];
	int				rows;
	int				columns;
} location_t;

typedef struct _Font
{
	TTF_Font* file;
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;

	SDL_Color textColor;
	SDL_Rect textRect;
} font_t;

#endif /* TYPEDEFS_H_ */
