/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    bomb.h
**************************************************************************/

/*
    爆弾に関する定義
*/

#define INCLUDE_BOMB

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void setBomb(void);         // 爆弾を置く
void explodeBomb(void);     // 爆弾が爆発
void initBomb(void);        // 爆弾の初期化
