/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    player.h
**************************************************************************/

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_STRUCTS
#include "structs.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void initPlayer(void);              // プレイヤーの値を初期化
void goalPlayer(void);              // プレイヤーがゴール
void hitPlayer(uchar);              // プレイヤーへのダメージ
void walkPlayer(void);              // プレイヤーの移動
void playerMove(void);              // プレイヤーの行動
void changeDirectionPlayer(void);   // プレイヤーの移動方向を変更
