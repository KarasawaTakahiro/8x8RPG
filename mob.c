#include "mob.h"

mob_t mob[MOB_BORN_NUM];

// 敵の設置
void bornMob(){
    uchar i, x, y;

    for(i=0; i<MOB_BORN_NUM; i++){
        getRandomPassagePoint(&x, &y);
        initMob(&(mob[i]), x, y);
    }
}

/*
    MOBの初期化

    mob : 対象とするMOB
    x   : 初期座標
    y   : 初期座標
*/
void initMob(mob_t* m, uchar x, uchar y){
    if(obj_tbl[y][x] == ID_WALL)
    m->active = MOB_IDLE;
    m->knockback = FALSE;
    m->attack = MOB_ATACK;
    m->x = x;
    m->y = y;
    m->hp = MOB_HP;
    m->dir = DIR_UP;
    m->obj_id = ID_MOB;

    setObject(x, y, (*mob).obj_id);
}

// MOBの攻撃
void mobAttack(mob_t m){
    uchar fx, fy;

    getFrontCoord(m.x, m.y, m.dir, &fx, &fy);
    damage(fx, fy, m.attack);
}

// mobの進行方向を決める
void mobChangeDirection(mob_t* m){
    signed char dx = (*m).x - player.x;
    signed char dy = (*m).y - player.y;

    if(dx < 0){
        dx *= (-1);
        if(dy < 0){
            dy *= (-1);
            if(dx < dy) m->dir = DIR_UP;
            else if(dy <= dx) m->dir = DIR_RIGHT;
        }else if(0 <= dy){
            if(dx < dy) m->dir = DIR_DOWN;
            else if(dy <= dx) m->dir = DIR_RIGHT;
        }
    }else if(0 <= dx){
        if(dy < 0){
            dy *= (-1);
            if(dx < dy) m->dir = DIR_UP;
            else if(dy <= dx) m->dir = DIR_LEFT;
        }else if(0 <= dy){
            if(dx < dy) m->dir = DIR_DOWN;
            else if(dy <= dx) m->dir = DIR_LEFT;
        }
    }
}

// MOBの移動
void mobMove(){
    uchar x, y;
    uchar front;
    uchar i;
    mob_t *m;

    for(i=0; i<MOB_BORN_NUM; i++){
        m = &(mob[i]);
        // 移動可能のMOB
        if(0 < m->hp){
            if(m->knockback == KNOCKBACK){  // ノックバックチェック
                m->knockback = FALSE;
            }else{
                mobChangeDirection(m);
                front = searchFront(m->x, m->y, m->dir);
                if(front == ID_PASSAGE){
                    x = m->x;
                    y = m->y;
                    switch(m->dir){
                        case DIR_RIGHT: (m->x)++;  // 右
                                        break;
                        case DIR_UP: (m->y)++;     // 上
                                     break;
                        case DIR_LEFT: (m->x)--;   // 左
                                       break;
                        case DIR_DOWN: (m->y)--;   // 下
                                       break;
                    }
                    mvObject(x, y, m->x, m->y, m->obj_id);
                }else if(front == ID_PLAYER){
                    mobAttack(*m);
                }
            }
        }
    }
}

// MOBの死亡処理
void deadMob(mob_t* m){
    m->active = UNMOVE;
    m->hp = 0;
    rmObject(m->x, m->y, ID_MOB);
}

// Mobへのダメージ
void hitMob(uchar x, uchar y, uchar val){
    mob_t *m;

    findMob(x, y, &m);

    if(m->hp <= val){
        m->hp = 0;
        deadMob(m);
    }else{
        m->hp -= val;
        m->knockback = KNOCKBACK;
    }
}

// MOBを探す
void findMob(uchar x, uchar y, mob_t **m){
    uchar i;

    for(i=0; i<MOB_BORN_NUM; i++){
        if(mob[i].x == x && mob[i].y == y){
            *m = &(mob[i]);
            break;
        }
    }
}
