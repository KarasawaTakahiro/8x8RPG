#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "dungeon_gen.h"

int seed;

uchar nextDir(){
    return rand() % 4;
}

// 迷路を作成した範囲を更新する
void updateRange(uchar x, uchar y, uchar max[2], uchar min[2]){
    if(max[0] < x)  max[0] = x;
    if(max[1] < y)  max[1] = y;
    if(min[0] > x)  min[0] = x;
    if(min[1] > y)  min[1] = y;
}

// 進行方向が残っているかを確認
uchar scan(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y){
    if( (map[y][x+2] == ID_PASSAGE || FIELD_SZ-1 <= x+1)
     && (map[y+2][x] == ID_PASSAGE || FIELD_SZ-1 <= y+1)
     && (map[y][x-2] == ID_PASSAGE || x-1 <= 0)
     && (map[y-2][x] == ID_PASSAGE || y-1 <= 0)){
        return 0;
    }else{
        return 1;
    }
}

uchar getBranchPoint(uchar max, uchar min){
    uchar res;

    res = (rand() + min) % max;
    if(res % 2) res = res + 1 % 2;  // 奇数の時

    return res;
}

// 分岐点を探す
void findBranchPoint(uchar map[FIELD_SZ][FIELD_SZ], uchar *x, uchar *y, uchar max[2], uchar min[2]){
    do{
        *x = getBranchPoint(max[0], min[0]);
        *y = getBranchPoint(max[1], min[1]);
    }while(map[*x][*y] == ID_PASSAGE);
}

uchar checkdig(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y, uchar dir){
    switch(dir){
        case 0:
            if(map[y][x+2] == ID_WALL && x+1 < FIELD_SZ-1)
                return 1;
            break;
        case 1:
            if(map[y+2][x] == ID_WALL && y+1 < FIELD_SZ-1)
                return 1;
            break;
        case 2:
            if(map[y][x-2] == ID_WALL && 0 < x-1)
                return 1;
            break;
        case 3:
            if(map[y-2][x] == ID_WALL && 0 < y-1)
                return 1;
            break;
    }
    return 0;
}

void dig(uchar map[FIELD_SZ][FIELD_SZ], uchar* x, uchar* y, uchar dir, int* plen, uchar max[2], uchar min[2]){

    switch(dir){
        case 0: map[*y][++(*x)] = ID_PASSAGE; break;
        case 1: map[++(*y)][*x] = ID_PASSAGE; break;
        case 2: map[*y][--(*x)] = ID_PASSAGE; break;
        case 3: map[--(*y)][*x] = ID_PASSAGE; break;
    }
    (*plen)++;
    updateRange(*x, *y, max, min);
}

void genDungeon(uchar map[FIELD_SZ][FIELD_SZ]){
    uchar dir, x, y;
    int plen = 0;
    int max_plen = FIELD_SZ * FIELD_SZ  * 4 / 9;
    uchar max[2], min[2];

    srand(seed);

    x = (rand() + 1) % FIELD_SZ;
    y = (rand() + 1) % FIELD_SZ;

    map[y][x] = 0;
    plen ++;
    max[0] = min[0] = x;
    max[1] = min[1] = y;

    while(plen <= max_plen){
        if(scan(map, x, y)){
            dir = nextDir();
            if(checkdig(map, x, y, dir)){
                dig(map, &x, &y, dir, &plen, max, min);
            }
        }else{
            findBranchPoint(map, &x, &y, max, min);
        }
    }

}

