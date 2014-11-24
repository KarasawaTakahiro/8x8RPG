#include <stdlib.h>
#include "player.h"

// グローバル変数
uchar obj_tbl[FIELD_SZ][FIELD_SZ];
volatile player_t player;
volatile uchar marker_f;
volatile uchar playerMove_f;

// プレイヤーの値を初期化
void initPlayer(){
    uchar x, y;

    do{
        x = rand() % FIELD_SZ;
        y = rand() % FIELD_SZ;
    }while(obj_tbl[y][x] != ID_PASSAGE);

    player.x = x;
    player.y = y;
    player.attack = PLAYER_ATTACK;
    player.dir = DIR_RIGHT;
    player.max_hp = PLAYER_INIT_HP;
    player.hp = player.max_hp;
    player.obj_id = ID_PLAYER;
    setObject(player.x, player.y, player.obj_id);

    marker_f = MARKER_SHOW;

    initBomb();
}

// プレイヤーがゴール
void goalPlayer(){
    uchar max = player.max_hp;
    uchar cur = player.hp;

    user_init();

    player.max_hp = ++max;
    if(player.max_hp <= PLAYER_MAX_HP)
        player.max_hp = PLAYER_MAX_HP;
    player.hp = ++cur;
}

// プレイヤーへのダメージ
void hitPlayer(uchar val){
    if(player.hp <= val){
        player.hp = 0;
        gameover = 1;
    }else{
        player.hp -= val;
    }
}

/*
    プレイヤーの移動
    directionに従ってプレイヤーの座標を更新する
*/
void walkPlayer(){
    uchar x = player.x;
    uchar y = player.y;

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
    mvObject(x, y, player.x, player.y, player.obj_id);
}

/*
    プレイヤーの行動
 */
void playerMove(void) {
    uchar front, fx, fy;

    switch(sw){
        case SW_1:
            front = searchFront(player.x, player.y, player.dir);
            if(front ==  ID_PASSAGE){   // 目の前が壁
                walkPlayer();
            }else if(front == ID_MOB){  // 目の前が敵
                getFrontCoord(player.x, player.y, player.dir, &fx, &fy);
                damage(fx, fy, player.attack);
            }else if(front == ID_GOAL){ // ゴール
                goalPlayer();
            }
            playerMove_f = MOVED;
            marker_f = MARKER_HIDE;
            break;
        case SW_2:
            changeDirection();
            playerMove_f = UNMOVE;
            break;
        case SW_3:
            setBomb();
            playerMove_f = MOVED;
            marker_f = MARKER_HIDE;
            break;
    }
}

