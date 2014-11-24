/*
    ユーザ部分とゲームフレームワークで
    共通して使われる
*/
#define INCLUDED_COMMON

#ifndef INCLUDED_CONATANTS
#include "constants.h"
#endif

typedef unsigned char uchar;        // 1byte
typedef unsigned int  uint;         // 2byte
enum { BEEP_LOW, BEEP_HIGH };
// 変数
extern volatile uchar sw;           // SWの値
extern volatile uchar led[LED_SZ];  // 8x8LED
extern volatile uchar gameover;     // ゲームオーバーフラグ
extern volatile uchar flash;        // 点滅値
extern int seed;                    // シード値
// 関数
extern void _sound(uchar tone);
extern void user_init(void);        // ユーザ初期化関数
extern void user_main(void);        // ユーザメイン関数
extern void timer_1sec_comp(void);  // 1秒タイマ 1秒経過時に呼ばれる
extern void timer_1sec_start(void); // 1秒タイマ 開始
extern void timer_1sec_stop(void);  // 1秒タイマ 停止
extern void _wdt_reset(void);

