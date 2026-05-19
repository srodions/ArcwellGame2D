#ifndef G_MAP_H_
#define G_MAP_H_

#include <stdio.h>
#include <stdbool.h>
#include "g_constants.h"

typedef struct ARCF_Header arcf_header_t;
typedef struct ARCF_Entry arcf_entry_t;
typedef struct Tile tile_t;
typedef struct Map map_t;
typedef struct ObjectManager obj_manager_t;

map_t* G_MapInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager);
tile_t G_TileInit(int srcX, int srcY, int posX, int posY);
void G_ObjInit(obj_manager_t* pObjManager, enum OBJ_ID id, int bsx, int bsy, int btx, int bty, bool isAnim);
void G_ObjSetter(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager);

#endif /* G_MAP_H_ */
