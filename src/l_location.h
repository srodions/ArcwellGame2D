#ifndef L_LOCATION_H_
#define L_LOCATION_H_

#include <stdbool.h>

typedef struct _ARCF_Header arcf_header_t;
typedef struct _ARCF_Entry arcf_entry_t;
typedef struct _Tile tile_t;
typedef struct _Location location_t;
typedef struct _ObjectManager obj_manager_t;
enum OBJ_ID;

location_t* L_LocationInit(FILE* arcFile, arcf_header_t* pHeader, arcf_entry_t* pTable, obj_manager_t* pObjManager);
tile_t L_TileInit(int srcX, int srcY, int posX, int posY);
void L_ObjectInit(obj_manager_t* pObjManager, enum OBJ_ID id, int bsx, int bsy, int btx, int bty, bool isAnim);
void L_Destruct(location_t* pLocation, obj_manager_t* pObjManager);

#endif /* L_LOCATION_H_ */
