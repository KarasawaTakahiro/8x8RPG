#include <stdlib.h>
#include "player.h"

// グローバル変数
uchar obj_tbl[FIELD_SZ][FIELD_SZ];
player_t player;
uchar marker_f;
uchar playerMove_f;

// プレイヤーの値を初期化
void initPlayer(){
    uchar x, y;

    // スタート地点を設定
    do{
        // フィールドの通路からランダムな1点を選ぶ
        x = rand() % FIELD_SZ;
        y = rand() % FIELD_SZ;
    }while(obj_tbl[y][x] != ID_PASSAGE);

    // 初期値を設定
    player.x = x;
    player.y = y;
    player.attack = PLAYER_ATTACK;
    player.dir = DIR_RIGHT;
    player.max_hp = PLAYER_INIT_HP;
    player.hp = player.max_hp;
    player.obj_id = ID_PLAYER;

    setObject(player.x, player.y, player.obj_id);   // オブジェクトテーブルに登録
    marker_f = MARKER_SHOW;                         // マーカー表示フラグをON
    playerMove_f = UNMOVE;                          // プレイヤーの動作フラグをOFF
    initBomb();                                     // 爆弾を初期化
}

// プレイヤーがゴール
void goalPlayer(){
    uchar max = player.max_hp;                      // 現在の最大HPを記憶
    uchar cur = player.hp;                          // 現在のHPを記憶

    se(SE_TYPE_HIGH); se(SE_TYPE_BASIC);
    se(SE_TYPE_HIGH); se(SE_TYPE_BASIC);
    se(SE_TYPE_HIGH); se(SE_TYPE_BASIC);

    user_init();                                    // ゲームを初期化

    // プレイヤーの最大HPと現在のHPを+1
    player.max_hp = ++max;
    if(player.max_hp <= PLAYER_MAX_HP)
        player.max_hp = PLAYER_MAX_HP;
    player.hp = ++cur;
}

/*
    プレイヤーへのダメージ

    val : ダメージ値
*/
void hitPlayer(uchar val){
    if(player.hp <= val){           // ダメージが現在HP以上
        player.hp = 0;              // HPを0に
        gameover = TRUE;            // ゲームオーバーフラグをON
        se(SE_TYPE_BASIC);
        se(SE_TYPE_BASIC);
    }else{
        player.hp -= val;           // HPをマイナス
    }
}

/*
    プレイヤーの移動
    directionに従ってプレイヤーの座標を更新する
*/
void walkPlayer(){
    uchar x = player.x;                 // オブジェクトテーブルの移動用に値を保存
    uchar y = player.y;                 // オブジェクトテーブルの移動用に値を保存

    // プレイヤーの座標を移動
    switch(player.dir){
        case DIR_RIGHT: player.x++;     // 右
            break;
        case DIR_UP: player.y++;        // 上
            break;
        case DIR_LEFT: player.x--;      // 左
            break;
        case DIR_DOWN: player.y--;      // 下
            break;
    }
    mvObject(x, y, player.x, player.y, player.obj_id);  // オブジェクトテーブルの移動
}

/*
    プレイヤーの行動
 */
void playerMove(void) {
    uchar front, fx, fy;

    // スイッチの値で変わる
    switch(sw){
        case SW_1:                      // SW1
            front = searchFront(player.x, player.y, player.dir);    // 目の前のオブジェクトを取得
            if(front ==  ID_PASSAGE){   // 壁
                walkPlayer();
            }else if(front == ID_MOB){  // 敵
                getFrontCoord(player.x, player.y, player.dir, &fx, &fy);
                damage(fx, fy, player.attack);
                se(SE_TYPE_HIGH);
            }else if(front == ID_GOAL){ // ゴール
                goalPlayer();
            }
            playerMove_f = MOVED;       // 行動フラグをON
            marker_f = MARKER_HIDE;     // マーカーを消す
            break;
        case SW_2:                      // SW2
            changeDirectionPlayer();    // 方向転換
            playerMove_f = UNMOVE;      // 方向転換は行動としてみなさない
            break;
        case SW_3:                      // 同時押し
            setBomb();                  // 爆弾を設置
            playerMove_f = MOVED;
            marker_f = MARKER_HIDE;
            break;
    }
}

// プレイヤーの移動方向を変更
void changeDirectionPlayer(){
    player.dir = (player.dir + 1) % 4;  // 方向転換
    marker_f = MARKER_SHOW;             // マーカー表示
}

