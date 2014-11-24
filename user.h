/*
    ユーザ部分とゲームフレームワークで
    共通して使われる
*/
#define INCLUDED_USER

#ifndef INCLUDED_COMMON
#include "common.h"
#endif

#ifndef INCLUDED_STRUCTS
#include "structs.h"
#endif

#ifndef INCLUDED_DUNGEON_GEN
#include "dungeon_gen.h"
#endif

// プロトタイプ
static void playerMove(void);
static void updateLed(void);
void initField(void);
void changeDirection(void);
uchar searchFront(uchar, uchar, uchar);
void walk(void);
void showMarker(void);
void showDungeon(void);
void getFrontCoord(uchar, uchar, uchar, uchar*, uchar*);
void setObject(uchar, uchar, uchar);
void setBomb(void);
void convObjToField(void);
void bornMob(uchar, uchar);
void initPlayer();
void damage(uchar, uchar, uchar);
void hitPassage(uchar, uchar, uchar);
void hitMob(uchar, uchar, uchar);
void hitPlayer(uchar);
void deadMob(mob_t*);
void initBomb(void);
void mobMove(mob_t*);
void clearObjTbl(void);
void mvObject(uchar src_x, uchar src_y, uchar dist_x, uchar dist_y, uchar obj_id);
void goalPlayer();

