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

#include "player.h"
#include "mob.h"
#include "bomb.h"

// プロトタイプ
void playerMove(void);
void updateLed(void);
void initField(void);
void changeDirection(void);
uchar searchFront(uchar, uchar, uchar);
void showMarker(void);
void showDungeon(void);
void getFrontCoord(uchar, uchar, uchar, uchar*, uchar*);
void setObject(uchar, uchar, uchar);
void convObjToField(void);
void damage(uchar, uchar, uchar);
void hitPassage(uchar, uchar, uchar);
void clearObjTbl(void);
void mvObject(uchar, uchar, uchar, uchar, uchar);
uchar rmObject(uchar, uchar, uchar);

// 共通変数
extern uchar obj_tbl[FIELD_SZ][FIELD_SZ];
extern volatile player_t player;
extern volatile uchar marker_f;
extern volatile uchar playerMove_f;
extern mob_t mob;
extern volatile bomb_t bomb;
