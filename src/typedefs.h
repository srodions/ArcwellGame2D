#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#define MAX_ENTITIES 32
#define MAX_OBJECTS 32
#define MAX_MAP_COLUMNS 39
#define MAX_MAP_ROWS 12

typedef struct _GameState
{
	unsigned int 	screenW;
	unsigned int 	screenH;
	int 			targetFPS;
	int 			currentFPS;
	double 			targetFrameTime;
	double 			deltaTime;
	bool 			isRunning;
	bool 			isPaused;
	bool 			isDebugMode;
} gamestate_t;

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
	char				direction;
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
	SDL_Rect 	entityDest;
	int			entitiesCount;
	bool		isIdle[MAX_ENTITIES];
	bool		isChasing[MAX_ENTITIES];
	bool		isMoving[MAX_ENTITIES];
	bool		isFalling[MAX_ENTITIES];
	rtimer_t	aiTimer[MAX_ENTITIES];
	rtimer_t	animTimer[MAX_ENTITIES];
	e_sprite_t	sprites[MAX_ENTITIES];
	e_tform_t 	transforms[MAX_ENTITIES];
	e_vel_t		velocities[MAX_ENTITIES];
	e_ai_t		aiParams[MAX_ENTITIES];
} e_manager_t;

typedef struct _Tile
{
	SDL_Rect 	tileSrc;
	int			posX, posY;
} tile_t;

typedef struct _ObjectManager
{
	SDL_Rect 		objDest[MAX_OBJECTS];
	obj_sprite_t	sprites[MAX_OBJECTS];
	rtimer_t		animTimer[MAX_OBJECTS];
	bool 			isAnimated[MAX_OBJECTS];
	int				objCount;
} obj_manager_t;

typedef struct _Location
{
	SDL_Texture* 	tileMap;
	SDL_Rect		locationDest;
	FILE* 			locationFile;
	tile_t 			locationTiles[MAX_MAP_COLUMNS][MAX_MAP_ROWS];
	int				rows;
	int				columns;
	int 			leftWallLength;
	int 			rightWallLength;
	int				currentLocationIndex;
	bool			isNextLocation;
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
