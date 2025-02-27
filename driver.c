#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "driver.h"
#include "bootsector.c"
#include "FATentry.c"
#include "directory.c"

#define HIWORD(l)   ((WORD)(l))
#define LOWORD(l)   ((WORD)(((WORD)(l) >> 16) & 0xFFFF))

void main(){

    struct BPB BPB; 
    struct METADATA meta;
    struct CLUSTER_ENTRY clus;
    char* SecBuff = NULL;

    initBPB(&BPB);
    initMeta(&meta);
    initClus(&clus);
    
    meta.Filename = "FAT32disk.img";
    
    setBPB(&BPB, meta.Filename);

    // print BPB fields

    printBPB(&BPB);
    
    // determine the FAT Type

    detFatType(&BPB, &meta);

    meta.BytsPerSec = BPB.BPB_BytsPerSec;
    meta.SecPerClus = BPB.BPB_SecPerClus;

    //calculate first data sector

    meta.FirstDataSector = BPB.BPB_ResvdSecCnt + (BPB.BPB_NumFATs * meta.FATSz) + meta.RootDirSectors;
    
    // read the ROOT DIRECTORY

    DIR* root = NULL;

    // printf("FATType: \t\t%u\n", Meta.FATType);

    if (meta.FATType == 12 || meta.FATType == 16){          // FAT12 and FAT16 root directory

        // compute the first root dir sector number

        meta.FirstRootDirSecNum = BPB.BPB_ResvdSecCnt + (BPB.BPB_NumFATs * BPB.BPB_FATSz16);

        // print the # of root dir sectors and first root dir sector number

        printf("RootDirSectors: \t%u\n", meta.RootDirSectors);
        printf("FirstRootDirSecNum: \t%u\n", meta.FirstRootDirSecNum);

        loadData(meta.Filename, &SecBuff, meta.BytsPerSec * 4, meta.BytsPerSec * meta.FirstRootDirSecNum);

        root = FetchRootDirectory(&meta, SecBuff);

        // cleanup

        free(SecBuff);
        SecBuff = NULL;

    } else {                                                        // FAT32 root directory
        
        root = FetchRootDirectory2(&meta, &BPB);

        // cleanup

        free(SecBuff);
        SecBuff = NULL;
    }

    // print root dir entries
    if (root != NULL)
        printDirectory(root);
}


/*
void setContentsOfCluster(int N, AUX* AUX){

    /// sets the contents of a cluser N

    if (AUX -> FATType == 12){          // FAT12
        if (N & 0x0001){
            AUX -> FAT12ClusEntryVal = AUX -> FAT12ClusEntryVal << 4;       // cluster number is ODD
            *((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = (*((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) & 0x000F;
        
        } else{
            AUX -> FAT12ClusEntryVal = AUX -> FAT12ClusEntryVal & 0x0FFF;   // cluster number is EVEN
            *((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = (*((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) | AUX -> FAT12ClusEntryVal;
        }

        *((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = (* ((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) | AUX -> FAT12ClusEntryVal;
    }

    if (AUX -> FATType == 16 )          // FAT16
        *((WORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = AUX -> FAT16ClusEntryVal;
    
    else{                               // FAT32
        AUX -> FAT32ClusEntryVal = AUX -> FAT32ClusEntryVal & 0x0FFFFFFF;
        *((DWORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = (*((DWORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) & 0x0FFFFFFF;
        *((DWORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset]) = (*((DWORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) | AUX -> FAT32ClusEntryVal;
    }
}

*/

/*
void initFAT(AUX* AUX, BPB* BPB){

    /// initialises a new fat

    AUX -> RootDirSectors = ((BPB -> BPB_RootEntCnt * 32) + (BPB -> BPB_BytsPerSec - 1)) / BPB -> BPB_BytsPerSec;
    DWORD TmpVal1 = AUX -> DskSize - (BPB -> BPB_ResvdSecCnt + AUX -> RootDirSectors);
    DWORD TmpVal2 = (256 * BPB -> BPB_SecPerClus) + BPB -> BPB_NumFATs;

    if(AUX -> FATType == 32)
        TmpVal2 = TmpVal2 / 2;
    
    AUX -> FATSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;

    if (AUX -> FATType == 32){
        BPB -> BPB_FATSz16 = 0;
        BPB -> BPB_FATSz32 = AUX -> FATSz;

    } else {
        BPB -> BPB_FATSz16 = LOWORD(AUX -> FATSz);
        // no BPB_FATSz32 in FAT16 BPB
    }
}



uint64_t FirstSectorOfCluster(CLUSTER_ENTRY* entry, METADATA* meta){

    /// calculates the first sector of data cluster N

        return ((entry -> clusterNumber - 2) * meta -> SecPerClus) + meta -> FirstDataSector;
}
*/