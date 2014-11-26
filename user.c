#include <stdlib.h>
#include "user.h"

/*
    ゲームの根幹部分
*/

// グローバル変数
volatile uchar sw = 0;              // 押しボタン
volatile uchar led[LED_SZ];         // マトリクスLED
volatile uchar gameover = 0;        // ゲーム終了フラグ
volatile uchar flash = 0;           // LED点滅用変数
int seed;                           // SEED値
char s[100];

// ローカル変数
uint field[FIELD_SZ];               // フィールド
uchar obj_tbl[FIELD_SZ][FIELD_SZ];  // オブジェクトテーブル オブジェクトIDを入れる
player_t player;                    // プレイヤー
uchar marker_f;                     // プレイヤーの進行方向を表すマーカーの表示用フラグ
bomb_t bomb;                        // 爆弾
uchar playerMove_f;                 // プレイヤーが行動したかを表すフラグ
mob_t mob[MOB_BORN_NUM];            // 敵キャラ
uchar pre_sw;                       // SWの押しっぱなしを無効にするための変数

/**************************************
    フレームワークから呼ばれる関数群
**************************************/

// ゲームの初期化
void user_init(void)
{
    pre_sw = sw;                // スイッチの初期化
    gameover = FALSE;           // ゲームオーバーフラグ

    srand(seed);                // シード値の設定
    seed = rand();              // 次のシード値を設定

    clearObjTbl();              // オブジェクトテーブルをクリア
    initField();                // フィールド生成
    initPlayer();               // プレイヤーを初期化
    bornMob();                  // MOBの設置

    se(SE_TYPE_HIGH);
}

// ゲームのメイン処理
void user_main(void)
{
    playerMove_f = UNMOVE;      // プレイヤーの行動フラグをリセット
    if(pre_sw != sw){           // スイッチが変化した時
        pre_sw = sw;
        playerMove();           // プレイヤーの行動
    }
    if(playerMove_f == MOVED)   // プレイヤーが動作したら
        mobMove();          // MOBの行動
    updateLed();                // 8x8LED表示の更新
}

// LED表示の更新
void updateLed(void)
{
    showDungeon();              // ダンジョンの表示
    led[3] |= 0x10;             // プレイヤーの位置を表示
    showMarker();               // 方向を表すマーカーを表示

    led[7] = player.hp;
}

// 1秒タイマが1秒経過時に呼ばれる
void timer_1sec_comp(){
    if(bomb.set){                   // 爆弾がセットされている
        if(1 < bomb.timelimit){         // タイムリミットが残っている
            bomb.timelimit --;      // タイムリミット-1
        }else{                      // タイムリミットが残っていない
            explodeBomb();          // 爆発
            timer_1sec_stop();      // カウンタ停止
        }
    }
}

/**************************************
    オリジナル関数群
**************************************/

// フィールドの初期化
void initField(){
    signed char y;
    uchar x;

    genDungeon(obj_tbl);        // ダンジョンの生成

    _puts("\n");
    for(y=FIELD_SZ-1; 0<=y; y--){
        for(x=0; x<FIELD_SZ; x++){
            sprintf(s, "%s", obj_tbl[y][x] == ID_WALL ? "W" : "_");_puts(s);
        }
        _puts("\n");
    }
    _puts("\n");

    // ゴールの設置 フィールド内のランダムなID_PASSAGEをゴールにする
    do{
        x = rand() % FIELD_SZ;
        y = rand() % FIELD_SZ;
    }while(obj_tbl[y][x] != ID_PASSAGE);
    obj_tbl[y][x] = ID_GOAL;

}

// オブジェクトテーブルをフィールドに変換
void convObjToField(){
    uchar x, y;
    uint row;                                   // フィールドの任意の行

    // オブジェクトテーブルを参照していく
    for(y=0; y<FIELD_SZ; y++){                  // テーブルから行を取り出す
        row = 0x0000;                           // フィールドの１行
        for(x=0; x<FIELD_SZ; x++){              // 各行の値を参照
            row <<= 1;                          // 列送り
            if(obj_tbl[y][x] == ID_MOB)         // MOBが存在したら
                row |= (uint)flash;
            else if(obj_tbl[y][x] == ID_GOAL)   // GOALが存在したら
                row |= (uint)flash;
            else if(obj_tbl[y][x] != ID_PASSAGE)    // オブジェクトが存在したら
                row ++;                         // フィールドに反映
        }
        field[y] = row;                         // 行に当てはめる
    }
}

/*
    前方のオブジェクトIDを返す

    x   : 現在のX座標
    y   : 現在のY座標
    dir : 向いている方向

    return  : オブジェクトID
*/
uchar searchFront(uchar x, uchar y, uchar dir){
    uchar fx, fy;

    getFrontCoord(x, y, dir, &fx, &fy);         // 前方の座標を取得

    return obj_tbl[fy][fx];
}

// プレイヤーの進行方向を示すマーカーを表示する
void showMarker(){
    if(marker_f == MARKER_SHOW){                // マーカー表示フラグ
        switch(player.dir){
        case 0: led[3] |= 0x08;                 // 右
            break;
        case 1: led[4] |= 0x10;                 // 上
            break;
        case 2: led[3] |= 0x20;                 // 左
            break;
        case 3: led[2] |= 0x10;                 // 下
            break;
        }
    }
}

// ダンジョンの表示
void showDungeon(){
    uchar i;
    uint mask;          // マスク
    signed char curx;   // X方向の基準点
    signed char cury;   // Y方向の基準点

    convObjToField();

    for(i=0; i<LED_SZ; i++){
        led[i] = 0x00;      // 一度真っ白にする

        // フィールドの表示と，左右フィールド外の表示
        cury = player.y - 3;                                    // 画面の下を基準に
        if(player.x-3 < 0){                                     // 画面の左端がfieldの左外
            curx = player.x - 3;                                // 画面の左端を基準にする
            mask = (0xff00 << (-curx));
            led[i] = (uchar)((field[cury+i] & mask) >> (FIELD_SZ - LED_SZ - curx));    // fieldから必要分を切り取って，右に詰める

        }else if(0 <= player.x-3 && player.x+4 <= FIELD_MAX){   // 真ん中 フィールドの表示
            curx = player.x - 3;                                // 画面の左端を基準に
            mask = (0xff00 >> curx);                            // マスクの調整
            led[i] = (uchar)(((field[cury+i] & mask)) >> (FIELD_SZ - (LED_SZ + curx))); // fieldから必要分を切り取って，右に詰める

        }else if(FIELD_MAX < player.x+4){                       // 画面の右端がFIELD_MAXより右
            curx = player.x + 4;                                // 画面の右端を基準にする
            mask = (0x00ff >> (curx - FIELD_MAX + 2));          // マスクの調整
            led[i] = (uchar)((field[cury+i] & mask) << (curx - FIELD_MAX + 2)); // fieldから必要分を切り取って，左に寄せる
        }

        // フィールド外の上下表示
        if(FIELD_MAX < player.y+4){                 // 上
            cury = player.y + 4;                    // 画面の上端を基準に
            if((LED_MAX-1)-(cury-FIELD_MAX) <= i){  // フィールド外
                led[i] = 0x0000;
            }
        }else if(player.y-3 < 0){                   // 下
            cury = player.y - 3;                    // 画面の下端を基準に
            if(cury < 0 && i < (-cury)){            // フィールド外
                led[i] = 0x0000;
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
void getFrontCoord(uchar x, uchar y, uchar dir, uchar* fx, uchar* fy){
    switch(dir){
        case DIR_RIGHT:         // 右の値を返す
            *fx = x+1;  *fy = y;
            break;
        case DIR_UP:            // 上
            *fx = x;  *fy = y+1;
            break;
        case DIR_LEFT:          // 左
            *fx = x-1;  *fy = y;
            break;
        case DIR_DOWN:          // 下
            *fx = x;  *fy = y-1;
            break;
    }
}

/*
    オブジェクトテーブルにオブジェクトを登録する

    x       : 登録先X座標
    y       : 登録先Y座標
    obj_id  : 登録するオブジェクトID
*/
void setObject(uchar x, uchar y, uchar obj_id){
    obj_tbl[y][x] = obj_id;
}

/* 
    オブジェクトテーブルの座標から指定したオブジェクトを削除する

    x       : X座標
    y       : Y座標
    obj_id  : 削除するオブジェクトID（チェック用）

    return  : TRUE/FALSE 成功/失敗
*/
uchar rmObject(uchar x, uchar y, uchar obj_id){
    if(obj_tbl[y][x] == obj_id){        // 指定された地点が指定されたIDの時
        obj_tbl[y][x] = ID_PASSAGE;     // 削除 == PASSAGE にする
        return TRUE;                       // 成功
    }else{
        return FALSE;                       // 失敗
    }
}

/*
    任意の座標に任意のダメージを与える

    x       : X座標
    y       : Y座標
    value   : ダメージ値
*/
void damage(uchar x, uchar y, uchar value){
    switch(obj_tbl[y][x]){              // ダメージ対象の座標にあるオブジェクトIDが
        case ID_WALL:                   // 壁
            hitWall(x, y, value);
            break;
        case ID_MOB:                    // MOB
            hitMob(x, y, value);
            break;
        case ID_PLAYER:                 // プレイヤー
            hitPlayer(value);
            break;
    }
}

/*
    壁へのダメージ

    x   : X座標
    y   : Y座標
    val : ダメージ値
*/
void hitWall(uchar x, uchar y, uchar val){
    // 外壁以外は消滅
    if((0 <  x) && (x < FIELD_MAX) && (0 < y) && (y < FIELD_MAX)){
        rmObject(x, y, ID_WALL);
    }
}

/*
    オブジェクトを移動する

    src_x   : 移動元X座標
    src_y   : 移動元Y座標
    dist_x  : 移動先X座標
    dist_y  : 移動先Y座標
    obj_id  : 対象のオブジェクトID
*/
void mvObject(uchar src_x, uchar src_y, uchar dist_x, uchar dist_y, uchar obj_id){
    if(rmObject(src_x, src_y, obj_id))      // 削除できたら
        setObject(dist_x, dist_y, obj_id);  // 移動先にセット
}

// オブジェクトテーブルをクリア
void clearObjTbl(){
    uchar i, j;

    // オブジェクトテーブルの全てをID_PASSAGEに
    for(i=0; i<FIELD_SZ; i++){
        for(j=0; j<FIELD_SZ; j++){
            obj_tbl[j][i] = ID_PASSAGE;
        }
    }
}

// フィールドからPASSAGEのランダムな1点を得る
void getRandomPassagePoint(uchar* x, uchar* y){
    do{
        *x = rand() % FIELD_SZ;
        *y = rand() % FIELD_SZ;
        _wdt_reset();
    }while(obj_tbl[*y][*x] != ID_PASSAGE);
}
