#include "cachelab.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Debug

typedef struct
{
    bool vaild;
    int tag;
    unsigned char* data;

    int count;
}Line;


typedef struct 
{   int UsedLines;
    Line* lines;
}Set;

typedef struct{
    int hits;
    int misses;
    int evictions;
}CacheStats;

typedef struct{
    int s;
    int E;
    int b;
    int t;
}CacheParms;

typedef struct
{   CacheStats stats;
    CacheParms parms;
    Set* sets;
}Cache;


void allocateLine(Line* line,int b, int tag){
    line->vaild = false;
    line->tag = tag;
    line->data = (unsigned char*)calloc((1<<b), sizeof(unsigned char));

    line->count = 0;
}

void allocateSet(Set* set,int E, int b){
    set->lines = (Line*)calloc(E, sizeof(Line));
    set->UsedLines = 0;
    for(int i = 0;i < E;i++){
        allocateLine(&(set->lines[i]), b, i);
    }
}

void allocateCache(Cache* cache, int s, int E, int b){
    cache->parms.s = s;
    cache->parms.E = E;
    cache->parms.b = b;
    cache->parms.t = 32 - s - b;

    cache->stats.evictions = 0;
    cache->stats.hits = 0;
    cache->stats.misses = 0;


    int setsNum = 1<<s;
    cache->sets = (Set*)calloc(setsNum, sizeof(Set));
    for(int i = 0;i < setsNum;i++){
        allocateSet(&(cache->sets[i]), E, b);
    }
}

//  eg.s=3
//      1<<s    1000 
// -> 
//      1<<s-1  0111 
int getIndex(long int address, int s, int b){
    return (address >> b) & ((1<<s) - 1);
}

int getOffset(long int address, int b){
    return address & ((1<<b) - 1);
}

int getTag(long int address, int s, int b){
    return address >> (b + s);
}

void Loadhandler(Cache* cache,char* instr, int tag, int index, int offset, int btyes){
    #if defined(Debug)
        printf("%s", instr);
    #endif
    Set* set = &((cache->sets)[index]);
    Line* lines = set->lines;
    int E = cache->parms.E;
    for(int i = 0;i < E;i++){
        Line* line = &lines[i];
        if(line->vaild && line->tag == tag){
            cache->stats.hits++;
            #if defined(Debug)
                printf(" hit\n");
            #endif
            return;
        }
    }

    cache->stats.misses++;
    #if defined(Debug)
        printf(" miss");
    #endif

    if(set->UsedLines >= E){
        #if defined(Debug)
            printf(" eviction");
        #endif
        cache->stats.evictions++;
        int min = 0;
        for(int i = 0;i < E;i++){
            Line* line = &lines[i];
            if(line->count < min){
                min = line->count;
            }
        }
    }else{
        set->UsedLines++;
        int used = set->UsedLines;
        lines[used - 1].vaild = true;
        lines[used - 1].count = 0;
        lines[used - 1].tag = tag;
        for(int i = 0;i < used - 1;i++){
            lines[i].count++;
        }
    }
    #if defined(Debug)
        printf("\n");
    #endif
}

void Storehandler(Cache* cache, char* instr, int tag, int index, int offset, int btyes){
    printf("start to handle store\n");
}

void Modifyhandler(Cache* cache, char* instr, int tag, int index, int offset, int btyes){
    printf("start to handle modify\n");
}

void handler(char* instr, Cache* cache){
    char* instrPrint = (char*)malloc(32*sizeof(char));
    strcpy(instrPrint, instr);
    instrPrint[strlen(instr) -2] = '\0';

    char* addr = instr + 3, *btyes;
    for(int i = 0;i < strlen(addr);i++){
        if(addr[i] == ','){
            addr[i] = '\0';
            btyes = addr + i + 1;
            break;
        }
    }
    long int address = strtol(addr, NULL, 16);
    int bytes = atoi(btyes);

    int b = cache->parms.b;
    int s = cache->parms.s;

    int index = getIndex(address, s, b);
    int offset = getOffset(address, b);
    int tag = getTag(address, s, b);


    switch (instr[1])
    {
    case 'L':
        Loadhandler(cache, instrPrint, tag, index, offset, bytes);
        break;
    case 'S':
        Storehandler(cache, instrPrint ,tag, index, offset, bytes);
        break;
    case 'M':
        Modifyhandler(cache, instrPrint, tag, index, offset, bytes);
        break;
    default:
        break;
    }
}




int main(int argc, char*args[])
{
    int s = atoi(args[2]);
    int E = atoi(args[4]);
    int b = atoi(args[6]);
    printf("s:%d E:%d b:%d\n", s, E, b);
    Cache cache;
    allocateCache(&cache, s, E, b);


    FILE* file = fopen(args[8], "r");

    char buffer[32];

    while(fgets(buffer, sizeof(buffer), file) != NULL){
        if(buffer[0] == 'I'){
            continue;
        }
        handler(buffer, &cache);
    }


    printSummary(0, 0, 0);
    return 0;
}
