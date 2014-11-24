#include <stdlib.h>
#include "user.h"

// グローバル変数
volatile uchar sw = 0;      // 押しボタン
volatile uchar led[LED_SZ]; // マトリクスLED
volatile uchar gameover = 0;// ゲーム終了フラグ
volatile uchar flash = 0;
int seed;

// ローカル変数
volatile uint field[FIELD_SZ] = {0};
uchar obj_tbl[FIELD_SZ][FIELD_SZ] = {{0}};
volatile player_t player;
volatile uchar marker_f;
volatile bomb_t bomb;
volatile uchar playerMove_f;
mob_t mob;
uchar pre_sw;

// デバッグ変数
volatile uchar print = 0x00;
volatile uchar print1 = 0x00;
volatile uchar print2 = 0x00;


/*
    フレームワークから呼ばれる関数群
*/
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    pre_sw = sw;
    gameover = 0;

    srand(seed);
    seed = rand();

    bomb.obj_id = ID_BOMB;

    clearObjTbl();
    initField();
    initPlayer();
    playerMove_f = UNMOVE;
    bornMob(4, 3);
}
/*
    ユーザ処理
 */
void user_main(void)
{
    playerMove_f = UNMOVE;   // プレイヤーの行動フラグをリセット
    if(pre_sw != sw){
        pre_sw = sw;
        playerMove();
    }
    if(playerMove_f == MOVED)
        mobMove(&mob);
    updateLed();
}

/*
    LED表示の更新
 */
void updateLed(void)
{
    showDungeon();
    led[3] |= 0x10; // プレイヤーの位置
    // 方向
    showMarker();

    led[0] = mob.hp;
    //led[1] = (mob.x << 4) | mob.y;
    //led[6] = 

    led[7] = player.hp;
    //print2 = 0;
}

/*
    オリジナル関数群
*/
// フィールドの初期化
void initField(){
    uchar x, y;

    // ダンジョンの生成
    genDungeon(obj_tbl);
    /*
    for(y=0; y<FIELD_SZ; y++){
        for(x=0; x<FIELD_SZ; x++){
            if(y == 0 || y == FIELD_SZ-1 || x == 0 || x == FIELD_SZ-1)
                obj_tbl[y][x] = ID_WALL;
            else
                obj_tbl[y][x] = ID_PASSAGE;
        }
    }
    */

    // ゴールの設置
    do{
        x = rand() % FIELD_SZ;
        y = rand() % FIELD_SZ;
    }while(obj_tbl[y][x] != ID_PASSAGE);
    obj_tbl[y][x] = ID_GOAL;

}

// オブジェクトテーブルをフィールドに変換
void convObjToField(){
    uchar x, y;
    uint row;       // フィールドの任意の行

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

// 前方のオブジェクトIDを返す
uchar searchFront(uchar x, uchar y, uchar dir){
    uchar fx, fy;

    getFrontCoord(x, y, dir, &fx, &fy);

    return obj_tbl[fy][fx];
}

// プレイヤーの進行方向を示すマーカーを表示する
void showMarker(){
    if(marker_f == MARKER_SHOW){
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
        //led[7] = player.dir;
    }
}

// ダンジョンの表示
void showDungeon(){
    uchar i;
    uint mask;    // マスク
    signed char curx;  // X方向の基準点
    signed char cury;  // Y方向の基準点

    convObjToField();

    for(i=0; i<LED_SZ; i++){
        led[i] = 0x00;      // 一度真っ白にする

        // フィールドの表示と，左右フィールド外の表示
        cury = player.y - 3;  // 画面の下を基準に
        if(player.x-3 < 0){                                     // 画面の左端がfieldの左外
            curx = player.x - 3;                                // 画面の左端を基準にする
            mask = (0xff00 << (-curx));
            led[i] = (uchar)((field[cury+i] & mask) >> (FIELD_SZ - LED_SZ - curx));    // fieldから必要分を切り取って，右に詰める

        }else if(0 <= player.x-3 && player.x+4 <= FIELD_MAX){   // 真ん中 フィールドの表示
            curx = player.x - 3;                                // 画面の左端を基準に
            mask = (0xff00 >> curx);                        // マスクの調整
            led[i] = (uchar)(((field[cury+i] & mask)) >> (FIELD_SZ - (LED_SZ + curx))); // fieldから必要分を切り取って，右に詰める

        }else if(FIELD_MAX < player.x+4){                       // 画面の右端がFIELD_MAXより右
            curx = player.x + 4;                                // 画面の右端を基準にする
            mask = (0x00ff >> (curx - FIELD_MAX + 2));      // マスクの調整
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

// オブジェクトテーブルにオブジェクトを登録する
void setObject(uchar x, uchar y, uchar obj_id){
    obj_tbl[y][x] = obj_id;
}

// オブジェクトテーブルの座標から指定したオブジェクトを削除する
uchar rmObject(uchar x, uchar y, uchar obj_id){
    if(obj_tbl[y][x] == obj_id){
        obj_tbl[y][x] = ID_PASSAGE;
        return 1;
    }else{
        return 0;
    }
}

void timer_1sec_comp(){
    if(bomb.set){
        print = bomb.timelimit;
        if(bomb.timelimit){
            bomb.timelimit --;
        }else{
            explodeBomb();
            timer_1sec_stop();
        }
    }
}

// 任意の座標に任意のダメージを与える
void damage(uchar x, uchar y, uchar value){
    switch(obj_tbl[y][x]){
        case ID_PASSAGE:
            hitPassage(x, y, value);
            break;
        case ID_MOB:
            hitMob(x, y, value);
            break;
        case ID_PLAYER:
            hitPlayer(value);
            break;
    }
}

// 壁へのダメージ
void hitPassage(uchar x, uchar y, uchar val){
    // 外壁以外は消滅
    if((0 <  x) && (x < FIELD_MAX) && (0 < y) && (y < FIELD_MAX)){
        rmObject(x, y, ID_PASSAGE);
    }
}

// オブジェクトを移動する
void mvObject(uchar src_x, uchar src_y, uchar dist_x, uchar dist_y, uchar obj_id){
    if(rmObject(src_x, src_y, obj_id))
        setObject(dist_x, dist_y, obj_id);
}

// オブジェクトテーブルをクリア
void clearObjTbl(){
    uchar i, j;

    for(i=0; i<FIELD_SZ; i++){
        for(j=0; j<FIELD_SZ; j++){
            obj_tbl[j][i] = ID_PASSAGE;
        }
    }
}

