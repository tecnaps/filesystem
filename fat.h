#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t DDWORD;

typedef struct BPB BPB;
typedef struct AUX AUX;
typedef struct DIRECTORY_ENTRY DIRECTORY_ENTRY;
typedef struct DSKSZTOSECPERCLUS DSKSZTOSECPERCLUS;
typedef struct FSInfo FSInfo;
typedef struct CLUSTER CLUSTER;
typedef struct DIR DIR;

void initBPB (BPB*);
void initAUX (AUX*);
void setBPB (BPB*, AUX*);
DWORD calcFirstSectorOfCluster(int, BPB*, AUX*);
DIR* FetchRootDirectory(int, int, AUX*);
void counSectors(BPB*, AUX*);
void detFatType(BPB*, AUX*);
void calcFATEntry(int, BPB*, AUX*);
void fetchClusterEntry(int, int, AUX *);
void setContentsOfCluster(int, AUX*);
void initFAT(AUX*, BPB*);

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

struct AUX{
    char* image;                             
    WORD RootDirSectors;
    DWORD FATSz;
    DWORD FirstDataSector;
    DWORD TotSec;
    DWORD DataSec;
    uint64_t CountOfClusters;
    DWORD FATType;
    DWORD FATOffset;
    DWORD ThisFATEntOffset;
    uint64_t ThisFATSecNum;
    WORD FAT12ClusEntryVal;
    WORD FAT16ClusEntryVal;
    DWORD FAT32ClusEntryVal;
    char* SecBuff;                      // size BPB_BytsPerSec or 2 * BPB_BytsPerSec (FAT12)
    int bufferSize;
    DWORD DskSize;
    DWORD FirstRootDirSecNum;
};

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

struct DSKSZTOSECPERCLUS {
    DWORD DiskSize;
    BYTE SecPerClusVal;
};

struct FSInfo {                         // offset   size
    DWORD FSI_LeadSig;                  // 0        4
    char* FSI_Reserved1[480];           // 4        480
    DWORD FSI_StrucSig;                 // 484      4
    DWORD FSI_Free_Count;               // 488      4
    DWORD FSI_Nxt_Free;                 // 492      4
    char* FSI_Reserved2[12];            // 596      12
    DWORD FSI_TrailSig;                 // 508      4
};

DSKSZTOSECPERCLUS DskTableFAT16 [] = {
    {8400, 0},          // up to 4.1MB      0 SecPerClusVal trips an error
    {32680, 2},         // up to 16MB       1k cluster
    {262144, 4},        // up to 128 MB     2k cluster
    {524288, 8},        // up to 256 MB     4k cluster
    {1048576, 16},      // up to 512 MB     8k cluster
    {2097152, 32},      // up to 1 GB       16k cluster
    {4194304, 64},      // up to 2 GB       32k cluster
    {0xFFFFFFFF, 0}     // more than 2GB    0 SecPerClusVal trips an error
};

DSKSZTOSECPERCLUS DskTableFAT32 [] = {
    {66600, 0},         // up to 32,5 MB    0 SecPerClusVal trips an error
    {532480, 1},        // up to 260 MB     .5k cluster
    {16777216, 9},      // up to 8 GB       4k cluster
    {33554432, 16},     // up to 16 GB      8k cluster
    {67108864, 32},     // up to 32 GB      16k cluster
    {0xFFFFFFFF, 64}    // more than 32 GB  32k cluster
};

struct DIR {
    DIR* previous;
    DIRECTORY_ENTRY* entry;
    DIR* next;
};

struct CLUSTER {
    DWORD entry;
    CLUSTER* Next;
    CLUSTER* Previous;
};
