#define INCLUDED_DUNGEON_GEN

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

uchar nextDir();
void updateRange(uchar x, uchar y, uchar max[2], uchar min[2]);
uchar scanAround(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y);
uchar getBranchPoint(uchar max, uchar min);
void findBranchPoint(uchar map[FIELD_SZ][FIELD_SZ], uchar *x, uchar *y, uchar max[2], uchar min[2]);
uchar checkdig(uchar map[FIELD_SZ][FIELD_SZ], uchar x, uchar y, uchar dir);
void dig(uchar map[FIELD_SZ][FIELD_SZ], uchar* x, uchar* y, uchar dir, uint* plen, uchar max[2], uchar min[2]);
void genDungeon(uchar map[FIELD_SZ][FIELD_SZ]);

