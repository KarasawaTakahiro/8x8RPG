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
// フレームワークから呼ばれる関数群
void user_init(void);                   // ゲームの初期化
void user_main(void);                   // ゲームのメイン処理
void updateLed(void);                   // LED表示の更新
void timer_1sec_comp(void);             // 1秒タイマが1秒経過時に呼ばれる
// ゲーム内関数群
void initField(void);                   // フィールドの初期化
void convObjToField(void);              // オブジェクトテーブルをフィールドに変換
uchar searchFront(uchar, uchar, uchar); // 前方のオブジェクトIDを返す
void showMarker(void);                  // プレイヤーの進行方向を示すマーカーを表示する
void showDungeon(void);                 // ダンジョンを表示
void getFrontCoord(uchar, uchar, uchar, uchar*, uchar*);    // オブジェクトの前方の座標を取得する
void setObject(uchar, uchar, uchar);    // オブジェクトテーブルにオブジェクトを登録する
uchar rmObject(uchar, uchar, uchar);    // オブジェクトテーブルの座標から指定したオブジェクトを削除する
void damage(uchar, uchar, uchar);       // 任意の座標に任意のダメージを与える
void hitWall(uchar, uchar, uchar);      // 壁へのダメージ
void mvObject(uchar, uchar, uchar, uchar, uchar);   // オブジェクトを移動する
void clearObjTbl(void);                 // オブジェクトテーブルをクリア
void getRandomPassagePoint(uchar*, uchar*);         // フィールドからPASSAGEのランダムな1点を得る

// 共通変数
extern uchar obj_tbl[FIELD_SZ][FIELD_SZ];
extern player_t player;
extern uchar marker_f;
extern uchar playerMove_f;
extern mob_t mob[MOB_BORN_NUM];
extern bomb_t bomb;
