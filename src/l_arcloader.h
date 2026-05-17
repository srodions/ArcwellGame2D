#ifndef L_ARCLOADER_H_
#define L_ARCLOADER_H_

#include <stdint.h>

typedef struct _ARCF_Header arcf_header_t;
typedef struct _ARCF_MapEntryHeader arcf_mapheader_t;
typedef struct _ARCF_ObjEntryHeader arcf_objheader_t;
typedef struct _ARCF_Entry arcf_entry_t;

arcf_header_t* L_LoadHeader(FILE* arcFile);
arcf_entry_t* L_LoadLumpsTable(FILE* arcFile, arcf_header_t* pHeader);
void* L_LoadLump(FILE* arcFile, const char* lumpName, arcf_header_t* pHeader, arcf_entry_t* pTable, uint32_t* outSize);

#endif /* L_ARCLOADER_H_ */
