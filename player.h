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
