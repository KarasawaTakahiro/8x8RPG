/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    common.h
**************************************************************************/

/*
    ユーザ部分とゲームフレームワークで
    共通して使われる
*/
#define INCLUDED_COMMON

#include <stdio.h>

#ifndef INCLUDED_CONATANTS
#include "constants.h"
#endif

// typedef
typedef unsigned char uchar;        // 1byte
typedef unsigned int  uint;         // 2byte
// 変数
extern volatile uchar sw;           // SWの値
extern volatile uchar led[LED_SZ];  // 8x8LED
extern volatile uchar gameover;     // ゲームオーバーフラグ
extern volatile uchar flash;        // 点滅値
extern int seed;                    // シード値
extern char s[100];                 // シリアル出力用のバッファ
// 関数
extern void user_init(void);        // ユーザ初期化関数
extern void user_main(void);        // ユーザメイン関数
extern void timer_1sec_comp(void);  // 1秒タイマ 1秒経過時に呼ばれる
extern void timer_1sec_start(void); // 1秒タイマ 開始
extern void timer_1sec_stop(void);  // 1秒タイマ 停止
extern void _wdt_reset(void);       // ウォッチドッグタイマをリセット
extern void se(uchar);              // 効果音を鳴らす
extern void _puts(char*);           // シリアルに出力
