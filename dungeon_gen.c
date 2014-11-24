#include <stdlib.h>
#include "dungeon_gen.h"

int seed;

static uchar nextDir(void);
static void updateRange(uchar, uchar, uchar max[2], uchar min[2]);
static uchar scanAround(uchar map[FIELD_SZ][FIELD_SZ], uchar, uchar);
static uchar getBranchPoint(uchar, uchar);
static void findBranchPoint(uchar map[FIELD_SZ][FIELD_SZ], uchar*, uchar*, uchar max[2], uchar min[2]);
static uchar checkdig(uchar map[FIELD_SZ][FIELD_SZ], uchar, uchar, uchar);
static void dig(uchar map[FIELD_SZ][FIELD_SZ], uchar*, uchar*, uchar, uint*, uchar max[2], uchar min[2]);

// ランダムな方向を取得
uchar nextDir(){
    return rand() % 4;
}

/*
    迷路を作成した範囲を更新する

    x   : 現在座標X
    y   : 現在座標Y
    max : 最大値 [x, y]
    min : 最小値 [x, y]
*/
void updateRange(uchar x, uchar y, uchar max[2], uchar min[2]){
    if(max[0] < x)  max[0] = x;     //　最大値を更新
    if(max[1] < y)  max[1] = y;     //　最大値を更新
    if(min[0] > x)  min[0] = x;     //　最小値を更新
    if(min[1] > y)  min[1] = y;     //　最小値を更新
}

/*
    進行方向が残っているかを確認

    map : フィールド
    x   : 現在座標X
    y   : 現在座標Y

    return  : TURE/FALSE 残っている/いない
*/
uchar scanAround(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y){
    // 上下左右の4方向に対して，
    // 2マス先が通路かつ，1マス先が最外壁でない
    if( (map[y][x+2] == ID_PASSAGE || FIELD_SZ-1 <= x+1)
     && (map[y+2][x] == ID_PASSAGE || FIELD_SZ-1 <= y+1)
     && (map[y][x-2] == ID_PASSAGE || x-1 <= 0)
     && (map[y-2][x] == ID_PASSAGE || y-1 <= 0)){
        return FALSE;
    }else{
        return TRUE;
    }
}

/*
    分岐点を探す

    max : 現在到達した最大座標
    min : 現在到達した最小座標
 */
uchar getBranchPoint(uchar max, uchar min){
    uchar res;

    do{
        _wdt_reset();
        res = (rand() + min) % max;     // 範囲内でランダムな点を探す
    }while(res != 0 && res != 1);       // どちらも最外壁を指してしまうので不可
    if(res % 2) res = res + 1 % 2;      // 奇数の時

    return res;
}

/*
    分岐点を探す

    map : フィールド
    x   : 分岐点のX座標
    y   : 分岐点のY座標
    max : 最大値
    min : 最小値
 */
void findBranchPoint(uchar map[FIELD_SZ][FIELD_SZ], uchar *x, uchar *y, uchar max[2], uchar min[2]){
    do{
        *x = getBranchPoint(max[0], min[0]);
        *y = getBranchPoint(max[1], min[1]);
    }while(map[*x][*y] == ID_PASSAGE);
}

uchar checkdig(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y, uchar dir){
    switch(dir){
        case DIR_RIGHT:
            if(x+2 <= FIELD_SZ && map[y][x+2] == ID_WALL && x+1 < FIELD_SZ-1)
                return TRUE;
            break;
        case DIR_UP:
            if(y+2 <= FIELD_SZ && map[y+2][x] == ID_WALL && y+1 < FIELD_SZ-1)
                return TRUE;
            break;
        case DIR_LEFT:
            if(0 <= x-2 && map[y][x-2] == ID_WALL && 0 < x-1)
                return TRUE;
            break;
        case DIR_DOWN:
            if(0 <= y-2 && map[y-2][x] == ID_WALL && 0 < y-1)
                return TRUE;
            break;
    }
    return FALSE;
}

void dig(uchar map[FIELD_SZ][FIELD_SZ], uchar* x, uchar* y, uchar dir, uint* plen, uchar max[2], uchar min[2]){

    switch(dir){
        case DIR_RIGHT: map[*y][++(*x)] = ID_PASSAGE; break;
        case DIR_UP:    map[++(*y)][*x] = ID_PASSAGE; break;
        case DIR_LEFT:  map[*y][--(*x)] = ID_PASSAGE; break;
        case DIR_DOWN:  map[--(*y)][*x] = ID_PASSAGE; break;
    }
    (*plen)++;
    updateRange(*x, *y, max, min);
}

void genDungeon(uchar map[FIELD_SZ][FIELD_SZ]){
    uchar dir, x, y;
    uint plen = 0;
    uint max_plen = FIELD_SZ * FIELD_SZ  * 4 / 9;
    uchar max[2], min[2];

    srand(seed);

    // 初期化
    for(y=0; y<FIELD_SZ; y++){
        for(x=0; x<FIELD_SZ; x++){
            map[y][x] = ID_WALL;
        }
    }

    x = rand() % (FIELD_SZ-1) + 1;
    y = rand() % (FIELD_SZ-1) + 1;

    map[y][x] = ID_PASSAGE;
    plen ++;
    max[0] = min[0] = x;
    max[1] = min[1] = y;

    while(plen <= max_plen){
        _wdt_reset();
        if(scanAround(map, x, y)){
            dir = nextDir();
            if(checkdig(map, x, y, dir)){
                dig(map, &x, &y, dir, &plen, max, min);
            }

        }else{
            findBranchPoint(map, &x, &y, max, min);
        }

    }

}

