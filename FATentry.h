#include <stdlib.h>
#include "bootsector.h"

#ifndef FATENTRY_H
#define FATENTRY_H

typedef struct CLUSTER_ENTRY CLUSTER_ENTRY;

void initClus(CLUSTER_ENTRY*);
void loadData(char *, char **, int, uint64_t);
void calcFATEntry(BPB *, METADATA *, CLUSTER_ENTRY *);
void fetchClusterEntry(int, char *, CLUSTER_ENTRY *);

struct CLUSTER_ENTRY
{
    uint64_t clusterNumber;
    uint64_t FirstSectorOfCluster;
    DWORD FATOffset;
    DWORD ThisFATEntOffset;
    uint64_t ThisFATSecNum;
    WORD FAT12ClusEntryVal;
    WORD FAT16ClusEntryVal;
    DWORD FAT32ClusEntryVal;
    CLUSTER_ENTRY *next;
};

#endif