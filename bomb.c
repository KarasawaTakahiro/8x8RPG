/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    bomb.c
**************************************************************************/

/*
    爆弾に関する定義
*/

#include "bomb.h"

// 爆弾の初期化
void initBomb(){
    bomb.timelimit = BOMB_TIMELIMIT;
    bomb.attack = BOMB_ATACK;
    bomb.obj_id = ID_BOMB;
    bomb.set = FALSE;
}

// 爆弾を置く
void setBomb(){
    uchar fx, fy;

    // 爆弾を未設置 && 前方の確認
    if(bomb.set == FALSE && searchFront(player.x, player.y, player.dir) == ID_PASSAGE){
        getFrontCoord(player.x, player.y, player.dir, &fx, &fy);
        bomb.timelimit = BOMB_TIMELIMIT;    // タイムリミットをセット
        setObject(fx, fy, bomb.obj_id);     // 爆弾を設置
        bomb.x = fx;
        bomb.y = fy;
        bomb.set = TRUE;
        timer_1sec_start();
    }
}

// 爆弾が爆発
void explodeBomb(){
    signed char i, j, x, y;

    se(SE_TYPE_BASIC);
    se(SE_TYPE_LOW);

    // 消滅処理
    bomb.set = FALSE;
    rmObject(bomb.x, bomb.y, bomb.obj_id);

    // ダメージ処理
    for(i=-1; i<=1; i++){
        for(j=-1; j<=1; j++){
            x = bomb.x + j;
            y = bomb.y + i;
            damage(x, y, bomb.attack);
        }
    }
}

