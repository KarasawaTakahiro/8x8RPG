// bomb.h
/*
    爆弾に関する定義
*/

#define INCLUDE_BOMB

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void setBomb(void);
void explodeBomb(void);
void initBomb(void);
