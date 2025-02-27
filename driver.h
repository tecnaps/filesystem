#include "bootsector.h"
#include "FATentry.h"
#include "directory.h"

#ifndef FAT_H
#define FAT_H

//typedef struct AUX AUX;

typedef struct DSKSZTOSECPERCLUS DSKSZTOSECPERCLUS;
typedef struct FSInfo FSInfo;

//void setContentsOfCluster(int, AUX*);
//void initFAT(AUX*, BPB*);
//uint64_t calcFirstSectorOfCluster(CLUSTER_ENTRY*, METADATA*);

/*struct AUX{
    char* SecBuff;                      // size BPB_BytsPerSec or 2 * BPB_BytsPerSec (FAT12)
    char* image;                             
    DWORD DskSize;

};*/

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
#endif