#include "mob.h"

mob_t mob;

// 敵の設置
void bornMob(uchar x, uchar y){
    mob.active = MOB_IDLE;
    mob.knockback = FALSE;
    mob.attack = MOB_ATACK;
    mob.x = x;
    mob.y = y;
    mob.hp = MOB_HP;
    mob.dir = DIR_UP;
    mob.obj_id = ID_MOB;

    setObject(x, y, mob.obj_id);
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
void mobMove(mob_t *m){
    uchar x, y;
    uchar front;

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
                    case DIR_RIGHT: m->x++;  // 右
                                    break;
                    case DIR_UP: (m->y)++;     // 上
                                 break;
                    case DIR_LEFT: m->x--;   // 左
                                   break;
                    case DIR_DOWN: m->y--;   // 下
                                   break;
                }
                mvObject(x, y, m->x, m->y, m->obj_id);
            }else if(front == ID_PLAYER){
                mobAttack(*m);
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
    if(mob.hp <= val){
        mob.hp = 0;
        deadMob(&mob);
    }else{
        mob.hp -= val;
        mob.knockback = KNOCKBACK;
    }
}

