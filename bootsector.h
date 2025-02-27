#include <stdint.h>

#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t DDWORD;

typedef struct BPB BPB;
typedef struct METADATA METADATA;

void initBPB (BPB*);
void initMeta (METADATA*);
void setBPB (BPB*, char*);
void detFatType(BPB*, METADATA*);
void printBPB (BPB*);

/** BPB Structure
 * 
 * Entry            Offset  Size
 * BS_jmpBoot       0       3
 * BS_OEMName       3       8
 * BPB_BytsPerSec   11      2
 * BPB_SecPerClus   13      1
 * BPB_RsvdSecCnt   14      2
 * BPB_NumFATs      16      1
 * BPB_RootEntCnt   17      2
 * BPB_TotSec16     19      2
 * BPB_Media        21      1
 * BPB_FATSz16      22      2
 * BPB_SecPerTrk    24      2
 * BPB_NumHeads     26      2
 * BPB_HiddSec      28      4
 * BPB_TotSec32     32      4
 * 
 * FAT12 and FAT16 Structure (start at offset 36)
 * 
 * BS_DrvNum        36      1 
 * BS_Reserved1     37      1
 * BS_BootSig       38      1
 * BS_VolID         39      4
 * BS_VolLab        43      11
 * BS_FilSysType    54      8
 * 
 * FAT32 (start at offset 36)
 * 
 * BPB_FATSz32      36      4
 * BPB_ExtFlags     40      2
 * BPB_FSVer        42      2
 * BPB_RootClus     44      4
 * BPB_FSInfo       48      2
 * BPB_BkBootSec    50      2
 * BPB_Reserved     52      12
 * BS_DrvNum        64      1
 * BS_Reserved1     65      1  
 * BS_BootSig       66      1
 * BS_VolID         67      4
 * BS_VolLab        71      11
 * BS_FilSysType    82      8
*/

// FAT 12 and 16                    // offset   size

struct BPB {                            // offset   size
    WORD BS_jmpBoot;                    //  0       3
    char BS_OEMName[8];                 //  3       8
    WORD BPB_BytsPerSec;                //  11      2
    BYTE BPB_SecPerClus;                //  13      1
    WORD BPB_ResvdSecCnt;               //  14      2
    BYTE BPB_NumFATs;                   //  16      1
    WORD BPB_RootEntCnt;                //  17      2
    WORD BPB_TotSec16;                  //  19      2
    BYTE BPB_Media;                     //  21      1
    WORD BPB_FATSz16;                   //  22      2
    WORD BPB_SecPerTrk;                 //  24      2
    WORD BPB_NumHeads;                  //  26      2
    DWORD BPB_HiddSec;                  //  28      4
    DWORD BPB_TotSec32;                 //  32      4
    BYTE BS_DrvNum;                     //  36      1
    BYTE BS_Reserved1;                  //  37      1
    BYTE BS_BootSig;                    //  38      1
    DWORD BS_VolID;                     //  39      4
    unsigned char BS_VolLab[11];        //  43      11
    char BS_FilSysType[8];              //  54      8
    DWORD BPB_FATSz32;                  //  36      4
    WORD BPB_ExtFlags;                  //  40      2
    WORD BPB_FSVer;                     //  42      2
    DWORD BPB_RootClus;                 //  44      4
    WORD BPB_FSInfo;                    //  48      2
    WORD BPB_BkBootSec;                 //  50      2
    unsigned char BPB_Reserved[12];    //  52      12
};

struct METADATA {
    char* Filename;
    WORD BytsPerSec;
    BYTE SecPerClus;
    DWORD RootDirSectors;
    DWORD FirstRootDirSecNum;
    DWORD FATSz;
    DWORD TotSec;
    DWORD DataSec;
    DWORD FirstDataSector;
    uint64_t CountOfClusters;
    DWORD FATType;
};

    #endif