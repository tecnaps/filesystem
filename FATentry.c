#include <stdio.h>
#include "bootsector.h"
#include "FATentry.h"

void initClus(CLUSTER_ENTRY* clus){

    clus -> clusterNumber = 0;
    clus -> FirstSectorOfCluster = 0;
    clus -> FATOffset = 0;
    clus -> ThisFATEntOffset = 0;
    clus -> ThisFATSecNum = 0;
    clus -> FAT12ClusEntryVal = 0;
    clus -> FAT16ClusEntryVal = 0;
    clus -> FAT32ClusEntryVal = 0;
    clus -> next = NULL;
}


void loadData(char* Filename, char** SecBuff, int BufferSize, uint64_t Offset){

    // note: the correct Buffersize for FAT12 is the caller's responsibility

    // free old SecBuffer buffer and initialize new SecBuffer

    if (*SecBuff != NULL)
        free(*SecBuff);

    *SecBuff = calloc(BufferSize, sizeof(char));

    // open Filename

    FILE* fd = fopen(Filename, "rb");

    if (fd == NULL){
        printf("Could not open image.\n");
        exit(-1);
    }
 
    // move filepointer to Offset 

    if (Offset != 0)
        fseeko(fd, Offset, SEEK_SET);
    
    // read data and close Filename

    fread(*SecBuff, sizeof(char), BufferSize,fd);

    fclose(fd);
}

void calcFATEntry(BPB* BPB, METADATA* meta, CLUSTER_ENTRY* entry){

    /// This method computes the location and offset of cluster entry on the disk

    // calculate FATOffset depending on entry size (12bit, 16bit or 32bit)

    if (meta -> FATType == 12) {                  // FAT12
        
        entry -> FATOffset = entry -> clusterNumber + (entry -> clusterNumber / 2);

        // calculate the sector number containing the entry
        entry -> ThisFATSecNum = BPB -> BPB_ResvdSecCnt + (entry -> FATOffset / BPB -> BPB_BytsPerSec);
        
        // calculate the byte offset of the entry inside ThisFATSecNum
        entry -> ThisFATEntOffset = entry -> FATOffset % BPB -> BPB_BytsPerSec;

    } else { 
                                               // FAT16 & FAT32
        if (meta -> FATType == 16)
            entry -> FATOffset = entry -> clusterNumber * 2;
        else 
            entry -> FATOffset = entry -> clusterNumber * 4;

        // calculate the sector number containing the entry
        entry -> ThisFATSecNum = BPB -> BPB_ResvdSecCnt + (entry -> FATOffset / BPB -> BPB_BytsPerSec);

        // calculate the byte offset of the entry inside ThisFATSecNum
        entry -> ThisFATEntOffset = entry -> FATOffset % BPB -> BPB_BytsPerSec;
    }    
}

void fetchClusterEntry(int FATType, char* SecBuff, CLUSTER_ENTRY* clus){

    if (FATType == 12){          // FAT12 * AUX -> bufferSize

        clus -> FAT12ClusEntryVal =  * ((WORD *) &SecBuff[clus -> ThisFATEntOffset]);

        if (clus -> clusterNumber & 0x0001)
            clus -> FAT12ClusEntryVal = clus -> FAT12ClusEntryVal >> 4;       // cluster number is ODD

        else
            clus -> FAT12ClusEntryVal = clus -> FAT12ClusEntryVal & 0x0FFF;   // cluster number is EVEN
    }

    else if (FATType == 16)      // FAT16
        clus -> FAT16ClusEntryVal = * ((WORD *) &SecBuff[clus->ThisFATEntOffset]);

    else                                // FAT32
        clus -> FAT32ClusEntryVal = (*((DWORD *) &SecBuff[clus -> ThisFATEntOffset])) & 0x0FFFFFFF;
}