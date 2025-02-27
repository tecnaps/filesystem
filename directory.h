#include "bootsector.h"
#ifndef DIRECTORY_H
#define DIRECTORY_H
//#include "FATentry.h"

typedef struct DIR DIR;
typedef struct DIRECTORY_ENTRY DIRECTORY_ENTRY;

DIR* FetchRootDirectory2(METADATA*, BPB*);
DIR* FetchRootDirectory(METADATA*, char*);
void printDirectory(DIR*);

struct DIRECTORY_ENTRY {                // offset   size
    char DIR_NAME[11];                 //  0       11
    BYTE DIR_Attr;                      //  11      1
    BYTE DIR_NTRes;                     //  12      1
    BYTE DIR_CrtTimeTenth;              //  13      1
    WORD DIR_CrtTime;                   //  14      2
    WORD DIR_CrtDate;                   //  16      2
    WORD DIR_LstAccDate;                //  18      2
    WORD DIR_FstClusHI;                 //  20      2
    WORD DIR_WrtTime;                   //  22      2
    WORD DIR_WrtDate;                   //  24      2
    WORD DIR_FstClusLO;                 //  26      2
    DWORD DIR_FileSize;                 //  28      4
};

struct DIR {
    DIR* previous;
    DIRECTORY_ENTRY* entry;
    DIR* next;
};
#endif