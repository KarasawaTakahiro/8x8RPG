typedef unsigned char uchar;

uchar nextDir();
void updateRange(uchar x, uchar y, uchar max[2], uchar min[2]);
uchar scan(uchar x, uchar y);
uchar getBranchPoint(max, min);
void findBranchPoint(uchar *x, uchar *y, uchar max[2], uchar min[2]);
uchar checkdig(uchar x, uchar y, uchar dir);
void dig(uchar* x, uchar* y, uchar dir, int* plen, uchar max[2], uchar min[2]);
void dungeon();
