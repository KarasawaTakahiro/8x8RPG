#include "user.h"


// ローカル関数
static void MoveEnemy(void);
static void MoveBullet(void);
static void MoveFort(void);
static void UpdateLED(void);
// グローバル変数
volatile uchar sw = 0;      // 押しボタン
volatile uchar led[LED_SZ]; // マトリクスLED
volatile uchar gameover = 0;// ゲーム終了フラグ
// ローカル変数
volatile ulong field[FIELD_SZ] = {0};
volatile uchar player[2];

// プロトタイプ
void initField(void);

/*
    フレームワークから呼ばれる関数群
*/
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    gameover = 0;
    player[0] = 1;
    player[1] = 4;
    initField();
}
/*
    ユーザ処理
 */
void user_main(void)
{
    MoveEnemy();
    MoveBullet();
    MoveFort();
    UpdateLED();
}
/*
    敵の移動
 */
static void MoveEnemy(void)
{
}
/*
    弾の移動
 */
static void MoveBullet(void)
{
}

/*
    砲台
 */
static void MoveFort(void)
{
    switch(sw){
        case 1:
            if(player[0] < FIELD_SZ)
                player[0] ++;
            break;
    }
}
/*
    LED表示の更新
 */
static void UpdateLED(void)
{
    uchar i;
    ulong mask = 0xff000000;
    uchar cur = player[0]-3;

    for(i=0; i<8; i++){
        led[i] = (uchar)((field[i] & (mask >> cur)) >> (FIELD_SZ-cur-LED_SZ));
        if(cur < 0)
            led[i] |= 0xff << (cur * (-1) + 1);
        else if((FIELD_SZ - LED_SZ) < cur)
            led[i] |= ~(0xff << (cur - (FIELD_SZ-LED_SZ)));
        led[i] |= 0x20;
    }
}

/*
    オリジナル関数群
*/
void initField(){
    uchar i;
    for(i=0; i<FIELD_SZ; i++){
        /*
        if(i==0 || i==FIELD_SZ-1)
            field[i] = 0xffffffff;
        else
            field[i] = 0x80000001;
         */
        field[i] = ~0xfffffffe;
    }
}
