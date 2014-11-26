/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    mob.h
**************************************************************************/

#define INCLUDE_MOB

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void bornMob();                         // 敵の設置
void initMob(mob_t*, uchar, uchar);     // MOBの初期化
void mobAttack(mob_t);                  // MOBの攻撃
void mobChangeDirection(mob_t*);        // mobの進行方向を決める
void mobMove();                         // MOBの移動
void deadMob(mob_t*);                   // 敵の設置
void hitMob(uchar, uchar, uchar);       // Mobへのダメージ
void findMob(uchar, uchar, mob_t**);    // MOBを探す
