/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    dungeon_gen.h
**************************************************************************/

#define INCLUDED_DUNGEON_GEN

#ifndef INCLUDED_COMMON
#include "common.h"
#endif
#ifndef INCLUDED_USER
#include "user.h"
#endif

void genDungeon(uchar map[FIELD_SZ][FIELD_SZ]);     // ダンジョン生成

