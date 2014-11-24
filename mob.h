#define INCLUDE_MOB

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void deadMob(mob_t*);
void mobMove();
void mobAttack(mob_t);
void mobChangeDirection(mob_t*);
void hitMob(uchar, uchar, uchar);
void bornMob();
void initMob(mob_t*, uchar, uchar);
void findMob(uchar, uchar, mob_t**);
