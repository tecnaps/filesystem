#include <stdlib.h>
#include "bootsector.h"

// INITIALISATIONS

void initBPB (BPB* BPB){

    /// This method initialises all fields in BPB to 0

    BPB -> BS_jmpBoot = 0;
    for (int i = 0; i < 8; i++)
        BPB -> BS_OEMName[i] = 0;
    BPB -> BPB_BytsPerSec = 0;
    BPB -> BPB_SecPerClus = 0;
    BPB -> BPB_ResvdSecCnt = 0;
    BPB -> BPB_NumFATs = 0;
    BPB -> BPB_RootEntCnt = 0;          // maximum number of entries for root directory
    BPB -> BPB_TotSec16 = 0;
    BPB -> BPB_Media = 0;
    BPB -> BPB_FATSz16 = 0;
    BPB -> BPB_SecPerTrk = 0;
    BPB -> BPB_NumHeads = 0;
    BPB -> BPB_HiddSec = 0;
    BPB -> BPB_TotSec32 = 0;

    // FAT 12 and 16 fields
    
    BPB -> BS_DrvNum = 0;
    BPB -> BS_Reserved1 = 0;
    BPB -> BS_BootSig = 0;
    BPB -> BS_VolID = 0;
    for (int i = 0; i < 11; i++)
        BPB -> BS_VolLab[11];

    for (int i = 0; i < 8; i++)
        BPB -> BS_FilSysType[i] = 0;
    
    // FAT 32 fields
    
    BPB -> BPB_FATSz32 = 0;
    BPB -> BPB_ExtFlags = 0;
    BPB -> BPB_FSVer = 0;
    BPB -> BPB_RootClus = 0;
    BPB -> BPB_FSInfo = 0;
    BPB -> BPB_BkBootSec = 0;
    for (int i = 0; i < 12; i++)
        BPB -> BPB_Reserved[i] = 0;
}


void initMeta (METADATA* meta){

    /// This method initialises all fields in AUX to 0

    meta -> Filename = NULL;
    meta -> BytsPerSec = 0;
    meta -> SecPerClus = 0;
    meta -> RootDirSectors = 0;
    meta -> FirstRootDirSecNum = 0;
    meta -> FATSz = 0;
    meta -> TotSec = 0;
    meta -> DataSec = 0;
    meta -> FirstDataSector = 0;
    meta -> CountOfClusters = 0;
    meta -> FATType = 0;
}

// SETTER

void setBPB(BPB* BPB, char* Filename){

    /// This method loads the BootSector values

    char * SecBuff = NULL;

    loadData(Filename, &SecBuff, 512, 0);

    if (SecBuff == NULL){
        printf("SecBuff is null");
        exit(-1);
    }

    BPB -> BS_jmpBoot = BPB -> BS_jmpBoot   | SecBuff[0] 
                                            | ((WORD) SecBuff[1] << 8) 
                                            | ((WORD) SecBuff[2] << 16);

    for (int i = 0; i < 8; i++)
        BPB -> BS_OEMName[i] = SecBuff[3+i];

    BPB -> BPB_BytsPerSec = BPB->BPB_BytsPerSec | SecBuff[11] 
                                                | (WORD) SecBuff[12] << 8;

    BPB -> BPB_SecPerClus = BPB->BPB_SecPerClus | SecBuff[13];

    BPB -> BPB_ResvdSecCnt = BPB -> BPB_ResvdSecCnt | SecBuff[14] 
                                                    | (WORD) SecBuff[15] << 8;

    BPB -> BPB_NumFATs = BPB -> BPB_NumFATs | SecBuff[16];

    BPB -> BPB_RootEntCnt = BPB -> BPB_RootEntCnt   | SecBuff[17] 
                                                    | (WORD) SecBuff[18] << 8;

    BPB -> BPB_TotSec16 = BPB -> BPB_TotSec16   | SecBuff[19] 
                                                | (WORD) SecBuff[20] << 8;

    BPB -> BPB_Media = BPB->BPB_Media | SecBuff[21];

    BPB -> BPB_FATSz16 = BPB -> BPB_FATSz16 | SecBuff[22] 
                                            | (WORD) SecBuff[23] << 8;

    BPB -> BPB_SecPerTrk = BPB -> BPB_SecPerTrk | SecBuff[24] 
                                                | (WORD) SecBuff[25] << 8;

    BPB -> BPB_NumHeads = BPB -> BPB_NumHeads   | SecBuff[26] 
                                                | (WORD) SecBuff[27] << 8;

    BPB -> BPB_HiddSec = BPB -> BPB_HiddSec | (DWORD) SecBuff[28]
                                            | (DWORD) SecBuff[29] << 8
                                            | (DWORD) SecBuff[30] << 16
                                            | (DWORD) SecBuff[31] << 24;
    
    BPB -> BPB_TotSec32 = BPB -> BPB_TotSec32   | (DWORD) SecBuff[32]
                                                | (DWORD) SecBuff[33] << 8
                                                | (DWORD) SecBuff[34] << 16
                                                | (DWORD) SecBuff[35] << 24;

    if (BPB -> BPB_FATSz16 != 0){                             // FAT12 or FAT16

        BPB -> BS_DrvNum = BPB -> BS_DrvNum | SecBuff[36];
    
        BPB -> BS_Reserved1 = BPB -> BS_Reserved1 | SecBuff[37];
    
        BPB -> BS_BootSig = BPB -> BS_BootSig | SecBuff[38];

        BPB -> BS_VolID = BPB -> BS_VolID   | (DWORD) SecBuff[39]
                                            | (DWORD) SecBuff[40] << 8
                                            | (DWORD) SecBuff[41] << 16
                                            | (DWORD) SecBuff[42] << 24;

        for (int i = 0; i < 11; i++)
            BPB -> BS_VolLab[i] = SecBuff[43+i];

        for (int i = 0; i < 8; i++)
            BPB -> BS_FilSysType[i] = SecBuff[54+i];
        
    } else {                                            // FAT32

        BPB -> BPB_FATSz32 = BPB->BPB_FATSz32   | (DWORD) SecBuff[36]
                                                | (DWORD) SecBuff[37] << 8
                                                | (DWORD) SecBuff[38] << 16
                                                | (DWORD) SecBuff[39] << 24;

        BPB -> BPB_ExtFlags = BPB -> BPB_ExtFlags   | SecBuff[40] 
                                                    | (WORD) SecBuff[41] << 8;

        BPB -> BPB_FSVer = BPB -> BPB_FSVer | SecBuff[42] 
                                            | (WORD) SecBuff[43] << 8;


        BPB -> BPB_RootClus = BPB -> BPB_RootClus   | (DDWORD) SecBuff[44]
                                                    | (DDWORD) SecBuff[45] << 8
                                                    | (DDWORD) SecBuff[46] << 16
                                                    | (DDWORD) SecBuff[47] << 24;
        
        BPB -> BPB_FSInfo = BPB -> BPB_FSInfo   | SecBuff[48] 
                                                | (WORD) SecBuff[49] << 8;

        BPB -> BPB_BkBootSec = BPB -> BPB_BkBootSec | SecBuff[50] 
                                                    | (WORD) SecBuff[51] << 8;

        for (int i = 0; i < 12; i++)
            BPB -> BPB_Reserved[i] = (char) SecBuff[52+i];
    
        BPB -> BS_DrvNum = BPB -> BS_DrvNum | SecBuff[64];

        BPB -> BS_Reserved1 = BPB -> BS_Reserved1 | SecBuff[65];
        
        BPB -> BS_BootSig = BPB -> BS_BootSig | SecBuff[66];

        BPB -> BS_VolID = BPB -> BS_VolID   | (DWORD) SecBuff[67]
                                            | (DWORD) SecBuff[68] << 8
                                            | (DWORD) SecBuff[69] << 16
                                            | (DWORD) SecBuff[70] << 24;

        for (int i = 0; i < 11; i++)
            BPB -> BS_VolLab[i] = (char) SecBuff[71+i];

        for (int i = 0; i < 8; i++)
            BPB -> BS_FilSysType[i] = SecBuff[82+i];   
    }

    free(SecBuff);
}

void detFatType(BPB* BPB, METADATA* Meta){
    
    /** This method determines the FAT type based on the count of clusters in the data region.
     * 
     *  FAT Types: 
     * 
     *  FAT12       CountOfClusters < 4085
     *  FAT16       CountOfClusters < 65525
     *  FAT32       CountOfClusters >= 65525
     * 
    */

    // determine the count of sectors in data region
    
    if (BPB -> BPB_FATSz16 != 0)
         Meta -> FATSz = BPB -> BPB_FATSz16;
    else
         Meta -> FATSz = BPB -> BPB_FATSz32;
    
    
    if (BPB -> BPB_TotSec16 != 0)
        Meta -> TotSec = BPB -> BPB_TotSec16;
    else
        Meta -> TotSec = BPB -> BPB_TotSec32;

    Meta -> RootDirSectors = ((BPB -> BPB_RootEntCnt * 32) * (BPB -> BPB_BytsPerSec -1)) / BPB -> BPB_BytsPerSec;

    Meta -> DataSec = Meta -> TotSec - (BPB -> BPB_ResvdSecCnt + (BPB -> BPB_NumFATs * Meta -> FATSz) + Meta -> RootDirSectors); 

    // determine the count of clusters in data region (rounds down)

    Meta -> CountOfClusters = Meta -> DataSec / BPB -> BPB_SecPerClus;

    // determine the FAT type based on CountOfClusters

    if (Meta -> CountOfClusters < 4085)
        Meta -> FATType = 12;
    else if (Meta -> CountOfClusters < 65525)
        Meta -> FATType = 16;
    else
        Meta -> FATType = 32;
}

// PRINTER

void printBPB(BPB* BPB){

    /// This method prints the BootSector

    printf("\nGeneral fields\n");
    printf("BS_jmpBoot: \t\t%u\n", BPB -> BS_jmpBoot);
    printf("BS_OEMName: \t\t%s\n", BPB -> BS_OEMName);
    printf("BPB_BytsPerSec: \t%u\n", BPB -> BPB_BytsPerSec);
    printf("BPB_SecPerClus: \t%u\n", BPB -> BPB_SecPerClus);
    printf("BPB_ResvdSecCnt: \t%u\n", BPB -> BPB_ResvdSecCnt);
    printf("BPB_NumFATs: \t\t%u\n", BPB -> BPB_NumFATs);
    printf("BPB_RootEntCnt: \t%u\n", BPB -> BPB_RootEntCnt);
    printf("BPB_TotSec16: \t\t%u\n", BPB -> BPB_TotSec16);
    printf("BPB_Media: \t\t%u\n", BPB -> BPB_Media);
    printf("BPB_FATSz16: \t\t%u\n", BPB -> BPB_FATSz16);
    printf("BPB_SecPerTrk: \t\t%u\n", BPB -> BPB_SecPerTrk);
    printf("BPB_NumHeads: \t\t%u\n", BPB -> BPB_NumHeads);
    printf("BPB_HiddSec: \t\t%u\n", BPB -> BPB_HiddSec);
    printf("BPB_TotSec32: \t\t%u\n", BPB -> BPB_TotSec32);

    if (BPB -> BPB_FATSz16 != 0){

        printf("\nSpecial FAT12 and FAT16 fields:\n");
        printf("BS_DrvNum: \t\t%u\n", BPB -> BS_DrvNum);
        printf("BS_Reserved1: \t\t%u\n", BPB -> BS_Reserved1);
        printf("BS_BootSig: \t\t%u\n", BPB -> BS_BootSig);
        printf("BS_VolID: \t\t%u\n", BPB -> BS_VolID);
        printf("BS_VolLab: \t\t%s\n", BPB -> BS_VolLab);
        printf("BS_FilSysType: \t\t%s\n", BPB -> BS_FilSysType);        

    } else {

        printf("\nSpecial FAT32 fields:\n");
        printf("BPB_FATSz32: \t\t%u\n", BPB -> BPB_FATSz32);
        printf("BPB_ExtFlags: \t\t%u\n", BPB -> BPB_ExtFlags);
        printf("BPB_FSVer: \t\t%u\n", BPB -> BPB_FSVer);
        printf("BPB_RootClus: \t\t%u\n", BPB -> BPB_RootClus);
        printf("BPB_FSInfo: \t\t%u\n", BPB -> BPB_FSInfo);
        printf("BPB_BkBootSec: \t\t%u\n", BPB -> BPB_BkBootSec);
        printf("BPB_Reserved: \t\t%s\n", BPB -> BPB_Reserved);
        printf("BS_DrvNum: \t\t%u\n", BPB -> BS_DrvNum);
        printf("BS_Reserved1: \t\t%u\n", BPB -> BS_Reserved1);
        printf("BS_BootSig: \t\t%u\n", BPB -> BS_BootSig);
        printf("BS_VolID: \t\t%u\n", BPB -> BS_VolID);
        printf("BS_VolLab: \t\t%s\n", BPB -> BS_VolLab);
        printf("BS_FilSysType: \t\t%s\n", BPB -> BS_FilSysType);

    }
}