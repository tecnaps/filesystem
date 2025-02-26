#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fat.h"

#define HIWORD(l)   ((WORD)(l))
#define LOWORD(l)   ((WORD)(((WORD)(l) >> 16) & 0xFFFF))

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

void initAUX (AUX* AUX){

    /// This method initialises all fields in AUX to 0

    AUX -> RootDirSectors = 0;
    AUX -> FATSz = 0;
    AUX -> FirstDataSector = 0;
    AUX -> TotSec = 0;
    AUX -> DataSec = 0;
    AUX -> CountOfClusters = 0;
    AUX -> FATType = 0;
    AUX -> FATOffset = 0;
    AUX -> ThisFATEntOffset = 0;
    AUX -> ThisFATSecNum = 0;
    AUX -> FAT12ClusEntryVal = 0;
    AUX -> FAT16ClusEntryVal = 0;
    AUX -> FAT32ClusEntryVal = 0;
    AUX -> SecBuff = malloc(512*sizeof(char));
    AUX -> bufferSize = 512;
    AUX -> DskSize = 0;
    AUX -> FirstRootDirSecNum = 0;
}


void readBoot(AUX* AUX){

    /// This method reads Data from the image into SecBuff buffer

    FILE* fd = fopen(AUX -> image, "rb");

    if (fd == NULL){
        printf("%s not found.", fd);
        exit(-1);
    }

    fread(AUX -> SecBuff, sizeof(char), 512, fd);

    fclose(fd);
}

void setBPB(BPB* BPB, AUX* AUX){

    /// This method loads the BootSector values

    BPB -> BS_jmpBoot = BPB -> BS_jmpBoot | AUX -> SecBuff[0] | ((WORD)AUX-> SecBuff[1] << 8) | ((WORD)AUX -> SecBuff[2] << 16);

    for (int i = 0; i < 8; i++)
        BPB -> BS_OEMName[i] = AUX -> SecBuff[3+i];

    BPB -> BPB_BytsPerSec = BPB->BPB_BytsPerSec | AUX -> SecBuff[11] 
                                                | (WORD)AUX -> SecBuff[12] << 8;

    BPB -> BPB_SecPerClus = BPB->BPB_SecPerClus | AUX -> SecBuff[13];

    BPB -> BPB_ResvdSecCnt = BPB -> BPB_ResvdSecCnt | AUX -> SecBuff[14] 
                                                    | (WORD)AUX -> SecBuff[15] << 8;

    BPB -> BPB_NumFATs = BPB -> BPB_NumFATs | AUX -> SecBuff[16];

    BPB -> BPB_RootEntCnt = BPB -> BPB_RootEntCnt   | AUX -> SecBuff[17] 
                                                    | (WORD)AUX -> SecBuff[18] << 8;

    BPB -> BPB_TotSec16 = BPB -> BPB_TotSec16   | AUX -> SecBuff[19] 
                                                | (WORD)AUX -> SecBuff[20] << 8;

    BPB -> BPB_Media = BPB->BPB_Media | AUX -> SecBuff[21];

    BPB -> BPB_FATSz16 = BPB -> BPB_FATSz16 | AUX -> SecBuff[22] 
                                            | (WORD)AUX -> SecBuff[23] << 8;

    BPB -> BPB_SecPerTrk = BPB -> BPB_SecPerTrk | AUX -> SecBuff[24] 
                                                | (WORD)AUX -> SecBuff[25] << 8;

    BPB -> BPB_NumHeads = BPB -> BPB_NumHeads   | AUX -> SecBuff[26] 
                                                | (WORD)AUX -> SecBuff[27] << 8;

    BPB -> BPB_HiddSec = BPB -> BPB_HiddSec | (DWORD)AUX -> SecBuff[28]
                                            | (DWORD)AUX -> SecBuff[29] << 8
                                            | (DWORD)AUX -> SecBuff[30] << 16
                                            | (DWORD)AUX -> SecBuff[31] << 24;
    
    BPB -> BPB_TotSec32 = BPB -> BPB_TotSec32   | (DWORD)AUX -> SecBuff[32]
                                                | (DWORD)AUX -> SecBuff[33] << 8
                                                | (DWORD)AUX -> SecBuff[34] << 16
                                                | (DWORD)AUX -> SecBuff[35] << 24;

    if (BPB -> BPB_FATSz16 != 0){                             // FAT12 or FAT16

        BPB -> BS_DrvNum = BPB -> BS_DrvNum | AUX -> SecBuff[36];
    
        BPB -> BS_Reserved1 = BPB -> BS_Reserved1 | AUX -> SecBuff[37];
    
        BPB -> BS_BootSig = BPB -> BS_BootSig | AUX -> SecBuff[38];

        BPB -> BS_VolID = BPB -> BS_VolID   | (DWORD)AUX -> SecBuff[39]
                                            | (DWORD)AUX -> SecBuff[40] << 8
                                            | (DWORD)AUX -> SecBuff[41] << 16
                                            | (DWORD)AUX -> SecBuff[42] << 24;

        for (int i = 0; i < 11; i++)
            BPB -> BS_VolLab[i] = AUX -> SecBuff[43+i];

        for (int i = 0; i < 8; i++)
            BPB -> BS_FilSysType[i] = AUX -> SecBuff[54+i];
        
    } else {                                            // FAT32

        BPB -> BPB_FATSz32 = BPB->BPB_FATSz32   | (DWORD)AUX -> SecBuff[36]
                                                | (DWORD)AUX -> SecBuff[37] << 8
                                                | (DWORD)AUX -> SecBuff[38] << 16
                                                | (DWORD)AUX -> SecBuff[39] << 24;

        BPB -> BPB_ExtFlags = BPB -> BPB_ExtFlags   | AUX -> SecBuff[40] 
                                                    | (WORD)AUX -> SecBuff[41] << 8;

        BPB -> BPB_FSVer = BPB -> BPB_FSVer | AUX -> SecBuff[42] 
                                            | (WORD)AUX -> SecBuff[43] << 8;


        BPB -> BPB_RootClus = BPB -> BPB_RootClus   | (DDWORD)AUX -> SecBuff[44]
                                                    | (DDWORD)AUX -> SecBuff[45] << 8
                                                    | (DDWORD)AUX -> SecBuff[46] << 16
                                                    | (DDWORD)AUX -> SecBuff[47] << 24;
        
        BPB -> BPB_FSInfo = BPB -> BPB_FSInfo   | AUX -> SecBuff[48] 
                                                | (WORD)AUX -> SecBuff[49] << 8;

        BPB -> BPB_BkBootSec = BPB -> BPB_BkBootSec | AUX -> SecBuff[50] 
                                                    | (WORD)AUX -> SecBuff[51] << 8;

        for (int i = 0; i < 11; i++)
            BPB -> BPB_Reserved[i] = (char) AUX -> SecBuff[52+i];
    
        BPB -> BS_DrvNum = BPB -> BS_DrvNum | AUX -> SecBuff[64];

        BPB -> BS_Reserved1 = BPB -> BS_Reserved1 | AUX -> SecBuff[65];
        
        BPB -> BS_BootSig = BPB -> BS_BootSig | AUX -> SecBuff[66];

        BPB -> BS_VolID = BPB -> BS_VolID   | (DWORD)AUX -> SecBuff[67]
                                            | (DWORD)AUX -> SecBuff[68] << 8
                                            | (DWORD)AUX -> SecBuff[69] << 16
                                            | (DWORD)AUX -> SecBuff[70] << 24;

        for (int i = 0; i < 11; i++)
            BPB -> BS_VolLab[i] = (char) AUX -> SecBuff[71+i];

        for (int i = 0; i < 8; i++)
            BPB -> BS_FilSysType[i] = AUX -> SecBuff[82+i];   
    }

}

void detFatType(BPB* BPB, AUX* AUX){
    
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

    /*    
    if (BPB -> BPB_FATSz16 != 0)
        AUX -> FATSz = BPB -> BPB_FATSz16;
    else
        AUX -> FATSz = BPB -> BPB_FATSz32;
    */
    
    if (BPB -> BPB_TotSec16 != 0)
        AUX -> TotSec = BPB -> BPB_TotSec16;
    else
        AUX -> TotSec = BPB -> BPB_TotSec32;

    AUX -> DataSec = AUX -> TotSec - (BPB -> BPB_ResvdSecCnt + (BPB -> BPB_NumFATs * AUX -> FATSz) + AUX -> RootDirSectors); 

    // determine the count of clusters in data region (rounds down)

    AUX -> CountOfClusters = AUX -> DataSec / BPB -> BPB_SecPerClus;

    // determine the FAT type based on CountOfClusters

    if (AUX -> CountOfClusters < 4085)
        AUX -> FATType = 12;
    else if (AUX -> CountOfClusters < 65525)
        AUX -> FATType = 16;
    else
        AUX -> FATType = 32;
}


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
        printf("BPB_Reserved: \t\t%u\n", BPB -> BPB_Reserved);
        printf("BS_DrvNum: \t\t%u\n", BPB -> BS_DrvNum);
        printf("BS_Reserved1: \t\t%u\n", BPB -> BS_Reserved1);
        printf("BS_BootSig: \t\t%u\n", BPB -> BS_BootSig);
        printf("BS_VolID: \t\t%u\n", BPB -> BS_VolID);
        printf("BS_VolLab: \t\t%s\n", BPB -> BS_VolLab);
        printf("BS_FilSysType: \t\t%s\n", BPB -> BS_FilSysType);

    }
}

void readData(uint64_t sector, AUX *AUX){

    FILE* fd = fopen(AUX -> image, "rb");

    if (fd == NULL){
        printf("Could not open image.\n");
        exit(-1);
    }
 
    fseeko(fd, sector * AUX -> bufferSize, SEEK_SET);
    
    if (AUX -> FATType == 12)

        // FAT12 entries might overlap sector boundaries

        fread(AUX -> SecBuff, sizeof(BYTE), 2 * AUX -> bufferSize,fd);
    else

        fread(AUX -> SecBuff, sizeof(BYTE),AUX -> bufferSize,fd);

    fclose(fd);
}


void main(){

    struct BPB BPB; 
    struct AUX AUX;
    initBPB(&BPB);
    initAUX(&AUX);
    
    // read BPB and set BPB struc

    AUX.image = "disk.img";
    readBoot(&AUX);
    setBPB(&BPB, &AUX);

    // print BPB fields

    printBPB(&BPB);

    // calculate First sector of data region

    AUX.RootDirSectors = ((BPB.BPB_RootEntCnt * 32) * (BPB.BPB_BytsPerSec -1)) / BPB.BPB_BytsPerSec;

    if (BPB.BPB_FATSz16 != 0)
        AUX.FATSz = BPB.BPB_FATSz16;
    else
        AUX.FATSz = BPB.BPB_FATSz32;

    AUX.FirstDataSector = BPB.BPB_ResvdSecCnt + (BPB.BPB_NumFATs * AUX.FATSz) + AUX.RootDirSectors;

    // print # of RootDirSectors and the FirstDataSector
        printf("\nRootDirSectors: \t%u\n", AUX.RootDirSectors);
        printf("FirstDataSector: \t%u\n", AUX.FirstDataSector);

    // determine the FAT Type

    detFatType(&BPB, &AUX);

    // print CountOfSectors and CountOfClusters

        printf("CountOfSectors: \t%u\n", AUX.DataSec);
        printf("CountOfClusters: \t%u\n", AUX.CountOfClusters);


/*****************************************************************************/
    
    // ROOT DIR

    DIR* root;

    printf("FATType: \t\t%u\n", AUX.FATType);

    if (AUX.FATType == 12 || AUX.FATType == 16 || true){

        AUX.RootDirSectors = ((BPB.BPB_RootEntCnt * 32) + (BPB.BPB_BytsPerSec - 11)) / BPB.BPB_BytsPerSec;
        AUX.FirstRootDirSecNum = BPB.BPB_ResvdSecCnt + (BPB.BPB_NumFATs * BPB.BPB_FATSz16);

        printf("RootDirSectors: \t%u\n", AUX.RootDirSectors);
        printf("FirstRootDirSecNum: \t%u\n", AUX.FirstRootDirSecNum);

        root = FetchRootDirectory( AUX.FirstRootDirSecNum, BPB.BPB_RootEntCnt, &AUX);

    } else {
        
        // set SecBuff to Sector Size
        
        AUX.SecBuff = malloc(BPB.BPB_BytsPerSec * sizeof(char));
        AUX.bufferSize = BPB.BPB_BytsPerSec;

        // compute sector number and byte offset

        calcFATEntry(2,&BPB, &AUX);

        printf("ThisFATSecNum: \t\t%u\n", AUX.ThisFATSecNum);
        printf("ThisFATEntOffset: \t%u\n", AUX.ThisFATEntOffset);

    }

    // print root dir entries

    DIR* iterator = root;

    if (iterator != NULL)
        printf("\n%s\n", iterator -> entry -> DIR_NAME);

    while (iterator -> next != NULL)
        {
            iterator = iterator -> next;
            printf("%s\n", iterator -> entry -> DIR_NAME);
        }

/*****************************************************************************/
}


DIR* FetchRootDirectory (int firstSector, int RootDirEntries, AUX* AUX){

    // unused entries are set to 0x00)

    DIR* directory = malloc(sizeof(DIR));
    DIR* start = directory;
    
    bool isEOF =  false;
    
    int i = 0;

    while(!isEOF){

    readData(firstSector+i, AUX);

    if (i == 0)
        directory -> previous = directory;

    for (int j = 0; j < 512; j+=32){

        if ( AUX->SecBuff[j] == 0x00)
            {
                isEOF = true;
                directory = directory -> previous;
                directory -> next = NULL;
                break;
            }

        directory -> entry = malloc(sizeof(DIRECTORY_ENTRY));
        
        directory -> next = malloc(sizeof(DIR));

        directory -> next -> previous = directory;

        // reading directory entry

        for (int k = 0; k < 11; k++)
            directory -> entry -> DIR_NAME[k] = (char) AUX -> SecBuff[j+k];

        directory -> entry -> DIR_Attr = AUX -> SecBuff[j+11];
        directory -> entry -> DIR_NTRes = AUX -> SecBuff[j+12];
        directory -> entry -> DIR_CrtTimeTenth = AUX -> SecBuff[j+13];
        directory -> entry -> DIR_CrtTime = directory -> entry -> DIR_CrtTime   | AUX -> SecBuff[j+14] 
                                                                                | AUX -> SecBuff[j+15] << 8;
        directory -> entry -> DIR_CrtDate = directory -> entry -> DIR_CrtDate   | AUX -> SecBuff[j+16] 
                                                                                | AUX -> SecBuff[j+17] << 8;
        directory -> entry -> DIR_LstAccDate = directory -> entry -> DIR_LstAccDate | AUX -> SecBuff[j+18] 
                                                                                    | AUX -> SecBuff[j+19] << 8;
        directory -> entry -> DIR_FstClusHI = directory -> entry -> DIR_FstClusHI   | AUX -> SecBuff[j+20] 
                                                                                    | AUX -> SecBuff[j+21] << 8;
        directory -> entry -> DIR_WrtTime = directory -> entry -> DIR_WrtTime   | AUX -> SecBuff[j+22] 
                                                                                | AUX -> SecBuff[j+23] << 8;
        directory -> entry -> DIR_WrtDate = directory -> entry -> DIR_WrtDate   | AUX -> SecBuff[j+24] 
                                                                                | AUX -> SecBuff[j+25] << 8;
        directory -> entry -> DIR_FstClusLO = directory -> entry -> DIR_FstClusLO   | AUX -> SecBuff[j+26] 
                                                                                    | AUX -> SecBuff[j+27] << 8;
        directory -> entry -> DIR_FileSize = directory -> entry -> DIR_FileSize | AUX -> SecBuff[j+28]
                                                                                | AUX -> SecBuff[j+29] << 8
                                                                                | AUX -> SecBuff[j+30] << 16
                                                                                | AUX -> SecBuff[j+31] << 24;

        directory = directory -> next;
        }

        if (!isEOF)
            i++;
    }

    return start;
}


void calcFATEntry(int N, BPB* BPB, AUX* AUX){

    /// calculate entry point for FAT
    
    /*
    if (BPB -> BPB_FATSz16 != 0)
        AUX -> FATSz = BPB -> BPB_FATSz16;
    
    else
        AUX -> FATSz = BPB -> BPB_FATSz32;
    */

    // calculate FATOffset depending on entry size (12bit, 16bit or 32bit)

    if (AUX -> FATType == 12) {                  // FAT12
        
        AUX -> FATOffset = N + (N / 2);

        // calculate the sector number containing the entry
        AUX -> ThisFATSecNum = BPB -> BPB_ResvdSecCnt + (AUX -> FATOffset / BPB -> BPB_BytsPerSec);
        
        // calculate the byte offset of the entry inside ThisFATSecNum
        AUX -> ThisFATEntOffset = AUX -> FATOffset % BPB -> BPB_BytsPerSec;

    } else { 
                                               // FAT16 & FAT32
        if (AUX -> FATType == 16)
            AUX -> FATOffset = N * 2;
        else 
            AUX -> FATOffset = N * 4;

        // calculate the sector number containing the entry
        AUX -> ThisFATSecNum = BPB -> BPB_ResvdSecCnt + (AUX -> FATOffset / BPB -> BPB_BytsPerSec);

        // calculate the byte offset of the entry inside ThisFATSecNum
        AUX -> ThisFATEntOffset = AUX -> FATOffset % BPB -> BPB_BytsPerSec;
    }    
}

void fetchClusterEntry(int N, int offset, AUX *AUX){

    // read sector N
    
    //CLUSTER* chain = malloc(AUX ->bufferSize);
    //CLUSTER* cur = malloc(AUX -> bufferSize);

    bool EndOfFile = false;

    readData(N, AUX);

    /// fetches the contents of a cluster N

    //while (!EndOfFile){  


        if (AUX -> FATType == 12){          // FAT12 * AUX -> bufferSize
            if (N & 0x0001)
                AUX -> FAT12ClusEntryVal = AUX -> FAT12ClusEntryVal >> 4;       // cluster number is ODD

            else
                AUX -> FAT12ClusEntryVal = AUX -> FAT12ClusEntryVal & 0x0FFF;   // cluster number is EVEN
        }

        else if (AUX -> FATType == 16)      // FAT16
            AUX -> FAT16ClusEntryVal = * ((WORD *) AUX -> SecBuff[AUX->ThisFATEntOffset]);

        else                                // FAT32
            AUX -> FAT32ClusEntryVal = (*((DWORD *) AUX -> SecBuff[AUX -> ThisFATEntOffset])) & 0x0FFFFFFF;

            DIRECTORY_ENTRY* dir = malloc(sizeof(DIRECTORY_ENTRY));


        // chain up

    //}
}

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

DWORD calcFirstSectorOfCluster(int N, BPB* BPB, AUX* AUX){

    /// calculates the first sector of data cluster N

        return ((N - 2) * BPB -> BPB_SecPerClus) + AUX -> FirstDataSector;
}

