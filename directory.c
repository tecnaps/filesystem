#include <stdio.h>
#include <stdbool.h>
#include "FATentry.h"
#include "directory.h"


DIR* FetchRootDirectory2(METADATA* meta, BPB* BPB){

    DIR* directory = calloc(sizeof(DIR), 1);
    
    DIR* start = NULL;

    CLUSTER_ENTRY* clus = calloc(sizeof(CLUSTER_ENTRY), 1);
    
    char* SecBuff = NULL;

    clus -> clusterNumber = BPB -> BPB_RootClus;

    uint64_t chain[10] = {0,0,0,0,0,0,0,0,0,0};
    int i = 0;

    // constructing the cluster chain

    while (!(clus -> clusterNumber >= 0x0FFFFFF8) && clus -> clusterNumber != 0){

        chain[i] = clus -> clusterNumber;

        calcFATEntry(BPB, meta, clus);

        loadData(meta -> Filename, &SecBuff, meta -> BytsPerSec * meta -> SecPerClus, meta -> BytsPerSec * clus -> ThisFATSecNum);

        // fetch next cluster entry

        fetchClusterEntry(meta -> FATType, SecBuff, clus);      
    
        clus -> clusterNumber = clus -> FAT32ClusEntryVal;
        i++;
    }

    int j = 0;

    while (j < i){

        // calculate first data sector of cluster

        clus -> FirstSectorOfCluster = ((chain[j] - 2) * meta -> SecPerClus) + meta -> FirstDataSector;

        loadData(meta -> Filename, &SecBuff, meta -> BytsPerSec * meta -> SecPerClus, meta -> BytsPerSec * clus -> FirstSectorOfCluster);

        // fetch root directory entry
    
        if (j == 0){
            directory = FetchRootDirectory(meta, SecBuff);
            start = directory;
            if (start == NULL)
                exit(-1);

        }
        else {
            directory -> next = FetchRootDirectory(meta, SecBuff);
            directory -> next -> previous = directory;
        }
        j++;
    }
    return start;
}


DIR* FetchRootDirectory(METADATA* meta, char* SecBuff){

    /** This method is used to read the root directory of FAT12 / FAT16 drives */

    // unused entries are set to 0x00)

    DIR* directory = calloc(sizeof(DIR), 1);
    directory -> previous = directory;

    DIR* start = directory;
    
    bool isEOF =  false;
    
    int i = 0;

    while(!isEOF){

    for (int j = 0; j < 512; j+=32){

        if (SecBuff[j] == 0x00)
            {
                isEOF = true;
                directory = directory -> previous;

                // cleanup

                free(directory -> next);
                directory -> next = NULL;

                break;
            }

        directory -> entry = calloc(sizeof(DIRECTORY_ENTRY), 1);
        
        directory -> next = calloc(sizeof(DIR), 1);

        directory -> next -> previous = directory;

        // reading directory entry

        for (int k = 0; k < 11; k++)
            directory -> entry -> DIR_NAME[k] = SecBuff[j+k];

        directory -> entry -> DIR_Attr = SecBuff[j+11];
        directory -> entry -> DIR_NTRes = SecBuff[j+12];
        directory -> entry -> DIR_CrtTimeTenth = SecBuff[j+13];

        directory -> entry -> DIR_CrtTime = directory -> entry -> DIR_CrtTime   | SecBuff[j+14] 
                                                                                | SecBuff[j+15] << 8;


        directory -> entry -> DIR_CrtDate = directory -> entry -> DIR_CrtDate   | SecBuff[j+16] 
                                                                                | SecBuff[j+17] << 8;
        directory -> entry -> DIR_LstAccDate = directory -> entry -> DIR_LstAccDate | SecBuff[j+18] 
                                                                                    | SecBuff[j+19] << 8;
        directory -> entry -> DIR_FstClusHI = directory -> entry -> DIR_FstClusHI   | SecBuff[j+20] 
                                                                                    | SecBuff[j+21] << 8;
        directory -> entry -> DIR_WrtTime = directory -> entry -> DIR_WrtTime   | SecBuff[j+22] 
                                                                                | SecBuff[j+23] << 8;
        directory -> entry -> DIR_WrtDate = directory -> entry -> DIR_WrtDate   | SecBuff[j+24] 
                                                                                | SecBuff[j+25] << 8;
        directory -> entry -> DIR_FstClusLO = directory -> entry -> DIR_FstClusLO   | SecBuff[j+26] 
                                                                                    | SecBuff[j+27] << 8;
        directory -> entry -> DIR_FileSize = directory -> entry -> DIR_FileSize | SecBuff[j+28]
                                                                                | SecBuff[j+29] << 8
                                                                                | SecBuff[j+30] << 16
                                                                                | SecBuff[j+31] << 24;
                                                                              
        directory = directory -> next;
        }

        if (!isEOF)
            i++;
    }

    return start;
}

void printDirectory(DIR* iterator){

    /// This method prints the names of all entries of a directory

    if (iterator != NULL)
        printf("\n%s\n", iterator -> entry -> DIR_NAME);

    while (iterator -> next != NULL)
        {
            iterator = iterator -> next;
            printf("%s\n", iterator -> entry -> DIR_NAME);
        }

}