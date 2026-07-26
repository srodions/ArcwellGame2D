#ifndef R_RENDERER_H_
#define R_RENDERER_H_

#include <stdint.h>
#include <stdio.h>
#include "g_constants.h"
#include "typedefs.h"

typedef struct GameState gamestate_t;
typedef struct Map map_t;
typedef struct EntityManager e_manager_t;
typedef struct ObjectManager obj_manager_t;
typedef struct EntityConfig e_config_t;
typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_SpriteHeader arcf_spriteheader_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct RenderAsset renderasset_t;

extern uint32_t r_screenBuffer[SCR_LOGICAL_WIDTH * SCR_LOGICAL_HEIGHT];
extern renderasset_t r_mapAssets[MAX_SPRITES];
extern renderasset_t r_objAssets[MAX_SPRITES];
extern renderasset_t r_entAssets[MAX_SPRITES];
extern renderasset_t r_uiAssets[MAX_SPRITES];

void R_MoveSpriteToBuffer(const uint32_t* pixels, int spriteW, int spriteH, int posX, int posY);
void R_MoveAtlasSpriteToBuffer(const uint32_t* pixels, int atlasW, int posX, int posY, int srcX, int srcY, int srcW, int srcH, int flipX);
void R_LoadSpritesData(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable);
void R_PushLocation(map_t* pLocation, e_manager_t* pEntManager);
void R_PushObject(obj_manager_t* pObjManager, e_manager_t* pEntManager);
void R_PushEntity(e_manager_t* pEntManager);
void R_PushText(const char* text, int x, int y);
void R_PushUI(gamestate_t* pGameState, e_manager_t* pEntManager);
void R_PushScene(gamestate_t* pGameState, map_t* pLocation, obj_manager_t* pObjManager, e_manager_t* pEntManager);
void R_Anim_Spawn(e_manager_t* pEntManager, e_config_t* config, int i);
void R_Anim_Anger(e_manager_t* pEntManager, e_config_t* config, int i);
void R_Anim_Walk(e_manager_t* pEntManager, e_config_t* config, int i);
void R_Anim_Attack(e_manager_t* pEntManager, e_config_t* config, int i);
void R_Anim_Death(e_manager_t* pEntManager, e_config_t* config, int i);
void R_Destruct();

#endif /* R_RENDERER_H_ */
