#include "cachelab.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool isDetailed = false;

typedef struct
{
    bool vaild;
    int tag;
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

void countLowerPlusOne(Cache* cache, int i, Line* lines){
    int Icount = lines[i].count;
    for(int j = 0;j < cache->sets->UsedLines;j++){
        if(j == i){
            continue;
        }
        if(lines[j].count < Icount){
            lines[j].count++;
        }
    }
    lines[i].count = 0;
}

void countPlusOne(Cache* cache, int i, Line* lines){
    for(int j = 0;j < cache->sets->UsedLines;j++){
        if(j == i){
            continue;
        }
        lines[j].count++;
    }
}


bool isHit(Cache* cache, int index, int tag){
    Line* lines = (&((cache->sets)[index]))->lines;
    for(int i = 0;i < cache->parms.E;i++){
        Line* line = &lines[i];
        if(line->vaild && line->tag == tag){
            cache->stats.hits++;
            countLowerPlusOne(cache, i, lines);

            if(isDetailed){
                printf(" hit");
            }
            return true;
        }
    }
    return false;
}

int LRU(Cache* cache, Set* set, int E, Line* lines){
    int newIndex = 0;
    if(set->UsedLines >= E){
        cache->stats.evictions++;
        int max = -1;
        for(int i = 0;i < E;i++){
            Line* line = &lines[i];
            if(line->count > max){
                max = line->count;
                newIndex = i;
            }
        }
        if(isDetailed){
            printf(" eviction");
        }
    }else{
        set->UsedLines++;
        newIndex = set->UsedLines - 1;
    }

    return newIndex;
}


void Loadhandler(Cache* cache, int tag, int index, int offset, int btyes){
    Set* set = &((cache->sets)[index]);
    Line* lines = set->lines;
    int E = cache->parms.E;

    if(isHit(cache, index, tag)){
        return;
    }

    cache->stats.misses++;
    if(isDetailed){
        printf(" miss");
    }

    int newIndex = LRU(cache, set, E, lines);



    lines[newIndex].vaild = true;
    lines[newIndex].count = 0;
    lines[newIndex].tag = tag;
    countPlusOne(cache, newIndex, lines);
}

void Storehandler(Cache* cache, int tag, int index, int offset, int btyes){
    Loadhandler(cache, tag, index, offset, btyes);
}

void Modifyhandler(Cache* cache, int tag, int index, int offset, int btyes){
    Loadhandler(cache, tag, index, offset, btyes);
    Storehandler(cache, tag, index, offset, btyes);
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

     if(isDetailed)
        printf("%s", instrPrint+1);
    switch (instr[1])
    {
    case 'L':
        Loadhandler(cache, tag, index, offset, bytes);
        break;
    case 'S':
        Storehandler(cache, tag, index, offset, bytes);
        break;
    case 'M':
        Modifyhandler(cache, tag, index, offset, bytes);
        break;
    default:
        break;
    }
    if(isDetailed){
        printf("\n");
    }
}




int main(int argc, char*args[])
{
    int s = atoi(args[2]);
    int E = atoi(args[4]);
    int b = atoi(args[6]);

    for(int i = 1;i < strlen(args[1]);i++){
        if(args[1][i] == 'v'){
            isDetailed = true;
        }
    }

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


    printSummary(cache.stats.hits, cache.stats.misses, cache.stats.evictions);
    return 0;
}
