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
volatile uchar direction;
volatile uchar marker_f;

// プロトタイプ
void initField(void);
void changeDirection(void);
uchar searchFront(uchar, uchar, uchar);
void walk(void);

/*
    フレームワークから呼ばれる関数群
*/
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    gameover = 0;
    player[0] = 2;
    player[1] = 4;
    direction = 0;
    marker_f = 1;
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
    プレイヤーの移動
 */
static void MoveFort(void)
{
    switch(sw){
        case 1:
            if(searchFront(player[0], player[1], direction) == P)
                walk();
            break;
        case 2:
            changeDirection();
            break;
    }
}
/*
    LED表示の更新
 */
static void UpdateLED(void)
{
    uchar i;
    ulong mask = 0xff000000;    // マスク
    signed char curx = player[0]-3;  // 左端を基準にする
    signed char cury = player[1]-4;  // 上を基準に
    for(i=0; i<8; i++){
        led[i] = 0x00;
        // ダンジョンの表示
        if(0 < cury && cury < FIELD_SZ)
            led[i] = (uchar)((field[cury+i] & (mask >> curx)) >> (FIELD_SZ-curx-LED_SZ));  // フィールドから8x8LEDに表示する部分を切り取る
        if(curx < 0)     // フィールドの左端を壁で表示
            led[i] |= (0xff << (LED_SZ - (curx * (-1))));

        else if((FIELD_SZ - LED_SZ) < curx)  // フィールドの右端を壁で表示
            led[i] |= ~(0xff << (curx - (FIELD_SZ-LED_SZ)));
        if(i < (cury*(-1)))
            led[i] |= 0xff;
        // ダンジョンの表示　ここまで
    }

    led[3] |= 0x10; // プレイヤーの位置
    // direction
    if(marker_f){
        switch(direction){
        case 0: led[3] |= 0x08;
            break;
        case 1: led[4] |= 0x10;
            break;
        case 2: led[3] |= 0x20;
            break;
        case 3: led[2] |= 0x10;
            break;
        }
        led[0] = 0xaa;
        led[1] = direction;
    }
}

/*
    オリジナル関数群
*/
// フィールドの初期化
void initField(){
    uchar i;
    for(i=0; i<FIELD_SZ; i++){
        if(i==0 || i==FIELD_SZ-1)
            field[i] = 0xffffffff;
        else if(i % 3 == 0)
            field[i] = 0xc0000001;
        else
            field[i] = 0x80000001;
    }
}

// プレイヤーの移動方向を変更
void changeDirection(){
    direction = (direction + 1) % 4;
    marker_f = 1;
}

// 前方のオブジェクトを返す
// pos, direction
uchar searchFront(uchar x, uchar y, uchar dir){
    ulong mask = 0x80000000 >> x;   // マスク

    switch(dir){
        case 0:     // 右の値を返す
            return ((field[y] & (mask>>1)) >> (FIELD_SZ-(x+1)));
            break;
        case 1:     // 上
            return ((field[y+1] & mask) >> (FIELD_SZ - x));
            break;
        case 2:     // 左
            return ((field[y] & (mask<<1)) >> (FIELD_SZ-(x-1)));
            break;
        case 3:     // 下
            return ((field[y-1] & mask) >> (FIELD_SZ - x));
            break;
    }
}

/*
    プレイヤーの移動
*/
void walk(){
    switch(direction){
        case 0: player[0]++;    // 右
            break;
        case 1: player[1]++;    // 上
            break;
        case 2: player[0]--;    // 左
            break;
        case 3: player[1]--;    // 下
            break;
    }
    marker_f = 0;
}
