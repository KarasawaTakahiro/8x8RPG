#include "user.h"


// 構造体
typedef struct player_s{
    uchar dir;
    uchar x;
    uchar y;
    uchar hp;
    uchar obj_id;
}player_t;

typedef struct mob_s{
    uchar active;
    uchar atack;
    uchar x;
    uchar y;
    uchar hp;
    uchar obj_id;
} mob_t;

typedef struct bomb_s{
    uchar x;
    uchar y;
    uchar timelimit;
    uchar show_f;   // 表示フラグ
    uchar obj_id;
} bomb_t;
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
volatile player_t player;
volatile uchar marker_f;
volatile bomb_t bomb;
volatile uchar print = 0x00;
volatile uchar print1 = 0x00;
volatile uchar print2 = 0x00;

// プロトタイプ
void initField(void);
void changeDirection(void);
uchar searchFront(uchar, uchar, uchar);
void walk(void);
void showMarker(void);
void showDungeon(void);

/*
    フレームワークから呼ばれる関数群
*/
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    gameover = 0;

    player.x = 26;
    player.y = 2;
    player.dir = 0;
    player.obj_id = ID_PLAYER;
    marker_f = 1;

    bomb.show_f = 0;
    bomb.obj_id = ID_BOMB;

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
static void MoveFort(void) {
    switch(sw){
        case 1:
            if(searchFront(player.x, player.y, player.dir) == P)
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

    showDungeon();
    led[3] |= 0x10; // プレイヤーの位置
    // 方向
    showMarker();

    print = player.y;

    //led[0] = print;
    //led[6] = print2;
}

/*
    オリジナル関数群
*/
// フィールドの初期化
void initField(){
    uchar i;
    field[31] = 0b11111111111111111111111111111111;
    for(i=1; i<31; i++)
        if(i%2)
            field[i]  = 0b10000000000000000000000000001001;
        else
            field[i]  = 0b10011011011011011011011011000001;
    field[0]  = 0b11111111111111111111111111111111;

}

// プレイヤーの移動方向を変更
void changeDirection(){
    player.dir = (player.dir + 1) % 4;
    marker_f = 1;
}

// 前方のオブジェクトを返す
// pos, direction
uchar searchFront(uchar x, uchar y, uchar dir){
    ulong mask = (0x80000000 >> x);   // マスク

    switch(dir){
        case 0:     // 右の値を返す
            return ((field[y] & (mask>>1)) >> (FIELD_SZ - (x+2)));
            break;
        case 1:     // 上
            return ((field[y+1] & mask) >> (FIELD_SZ - (x+1)));
            break;
        case 2:     // 左
            return ((field[y] & (mask<<1)) >> (FIELD_SZ - x));
            break;
        case 3:     // 下
            return ((field[y-1] & mask) >> (FIELD_SZ - (x+1)));
            break;
    }
}

/*
    プレイヤーの移動
    directionに従ってプレイヤーの座標を更新する
*/
void walk(){
    switch(player.dir){
        case 0: player.x++;    // 右
            break;
        case 1: player.y++;    // 上
            break;
        case 2: player.x--;    // 左
            break;
        case 3: player.y--;    // 下
            break;
    }
    marker_f = 0;
}

// プレイヤーの進行方向を示すマーカーを表示する
void showMarker(){
    if(marker_f){
        switch(player.dir){
        case 0: led[3] |= 0x08;    // 右
            break;                      
        case 1: led[4] |= 0x10;    // 上
            break;                      
        case 2: led[3] |= 0x20;    // 左
            break;                      
        case 3: led[2] |= 0x10;    // 下
            break;
        }
        led[7] = player.dir;
    }
}

// ダンジョンの表示
void showDungeon(){
    uchar i;
    ulong mask;    // マスク
    signed char curx;  // X方向の基準点
    signed char cury;  // Y方向の基準点

    for(i=0; i<LED_SZ; i++){
        led[i] = 0x00;      // 一度真っ白にする

        // フィールドの表示と，左右フィールド外の表示
        cury = player.y - 3;  // 画面の下を基準に
        if(player.x-3 < 0){                                     // 画面の左端がfieldの左外
            curx = player.x - 3;                                // 画面の左端を基準にする
            mask = (0xff000000 << (-curx));
            led[i] = (uchar)((field[cury+i] & mask) >> (FIELD_SZ - LED_SZ - curx));    // fieldから必要分を切り取って，右に詰める

        }else if(0 <= player.x-3 && player.x+4 <= FIELD_MAX){   // 真ん中 フィールドの表示
            curx = player.x - 3;                                // 画面の左端を基準に
            mask = (0xff000000 >> curx);                        // マスクの調整
            led[i] = (uchar)(((field[cury+i] & mask)) >> (FIELD_SZ - (LED_SZ + curx))); // fieldから必要分を切り取って，右に詰める

        }else if(FIELD_MAX < player.x+4){                       // 画面の右端がFIELD_MAXより右
            curx = player.x + 4;                                // 画面の右端を基準にする
            mask = (0x000000ff >> (curx - FIELD_MAX + 2));      // マスクの調整
            led[i] = (uchar)((field[cury+i] & mask) << (curx - FIELD_MAX + 2)); // fieldから必要分を切り取って，左に寄せる
        }

        // フィールド外の上下表示
        if(FIELD_MAX < player.y+4){                 // 上
            cury = player.y + 4;                    // 画面の上端を基準に
            if((LED_MAX-1)-(cury-FIELD_MAX) <= i){  // フィールド外
                led[i] = 0x00000000;
            }
        }else if(player.y-3 < 0){                   // 下
            cury = player.y - 3;                    // 画面の下端を基準に
            if(cury < 0 && i < (-cury)){            // フィールド外
                led[i] = 0x00000000;
            }
        }
    }

}

/*
    オブジェクトの前方の座標を取得する
    x, y : オブジェクトの座標
    dir  : オブジェクトの方向
    fx, fy : 戻り用ポインタ
*/
void getFrontCoord(uchar x, uchar y, uchar dir, uchar *fx, uchar *fy){
    switch(dir){
        case 0:     // 右の値を返す
            *fx = x+1;  *fy = y;
            break;
        case 1:     // 上
            *fx = x;  *fy = y+1;
            break;
        case 2:     // 左
            *fx = x-1;  *fy = y;
            break;
        case 3:     // 下
            *fx = x;  *fy = y-1;
            break;
    }
}

// 爆弾を置く
void setBomb(){
    if(searchFront(player.x, player.y, player.dir) == P){
    }
}

