#include "cachelab.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    bool vaild;
    unsigned char* tag;
    unsigned char* data;
}Line;


typedef struct 
{
    Line* lines;
}Set;

typedef struct
{
    Set* sets;
}Cache;

void allocateLine(Line* line,int b, int t){
    line->vaild = false;
    line->tag = (unsigned char*)malloc(t*sizeof(unsigned char));
    line->data = (unsigned char*)malloc((1<<b)*sizeof(unsigned char));
}

void allocateSet(Set* set,int E, int b, int t){
    set->lines = (Line*)malloc((E)*sizeof(Line));
    for(int i = 0;i < E;i++){
        allocateLine(&(set->lines[i]), b, t);
    }
}

void allocateCache(Cache* cache, int s, int E, int b){
    u_int32_t num = 1<<s;
    int t = 32 - (b + s);
    
    cache->sets = (Set*)malloc((num) * sizeof(Set));
    for(int i = 0;i < num;i++){
        allocateSet(&(cache->sets[i]), E, b, t);
    }
}

int main()
{
    int s = 1, E = 2, b = 3;
    Cache cache;
    allocateCache(&cache, s, E, b);


    printSummary(0, 0, 0);
    return 0;
}
