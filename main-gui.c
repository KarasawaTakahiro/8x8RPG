/*
	main.c : ForCy-USB シミュレータ

	Copyright(C) 2011 Osamu Tamura @ Recursion Co., Ltd.
					All rights reserved.
*/

// UNIX Version (tcl/tk): Daisuke Takago 2014
//  gcc main.c user.c -lpthread -ltcl -ltk -I/usr/include/tk 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <tcl/tcl.h>
#include <tk/tk.h>
#include <pthread.h> 
#include "user.h"

/*
 *  構造体定義
 */
typedef struct SW{
    int in;  // スイッチ（オートリピートあり）
    int s;   // 状態
    int cnt;
    int out; // スイッチ（オートリピートなし)
} SW;

/* 
 * スレッド定義
 */
void *ThreadGUI(void *); // tcl/tkのGUIを描画するスレッド
void *ThreadKey(void *); // キー入力用のスレッド(オートリピートをキャンセルする機能がある)


/*
 *  変数定義
 */
struct SW sw_x, sw_z; // xとyキー
static unsigned char vled[8][8]; // M行，N列               

#define	PCBUFSZ		64
static int				pc_rptr;
static int				pc_wptr;
static unsigned char	pc_data[PCBUFSZ];

/*
 *  関数宣言
 */
static void			view_update();
static void key_check( void );
static int			uart_getc( void );
static int ar_cancel(struct SW *sw);// オートロリピートキャンセラー

/*
 *  関数定義
 */


/***********************************
 * メインルーチン
 ***********************************/
int 
main( int argc, char* argv[] )
{

	pthread_t th1,th2;
	pthread_create(&th2, NULL, ThreadGUI, argv[0]);
	pthread_create(&th1, NULL, ThreadKey, argv[0]);
    
    user_init();
    while(1){
        key_check();
        user_main();
		view_update();
        usleep(100000); // ここでスピード調整できる
    }
    
	pthread_join(th2, NULL);
	pthread_join(th1, NULL);
    return 0;
}


/***********************************
 * オートリピートをキャンセルしつつキーを読み取る
 ***********************************/
void *ThreadKey(void *arg){
    while(1){
        usleep(100);
        ar_cancel(&sw_x);
        ar_cancel(&sw_z);
    }
}

/***********************************
 * tclインタプリタを動かす(GUI本体)
 ***********************************/
void *ThreadGUI(void *arg)
{
 
    Tcl_Interp *interp = Tcl_CreateInterp();
    Tcl_FindExecutable((char *)arg);
    Tcl_Init(interp);
    Tk_Init(interp);
    /* c言語の変数とtclの変数を関連付ける */
    Tcl_LinkVar(interp, "z", (char *)(&sw_z.in), TCL_LINK_INT);
    Tcl_LinkVar(interp, "x", (char *)(&sw_x.in), TCL_LINK_INT);
    Tcl_LinkVar(interp, "zo", (char *)(&sw_z.out), TCL_LINK_INT);
    Tcl_LinkVar(interp, "xo", (char *)(&sw_x.out), TCL_LINK_INT);

    int m,n;
    char buff[256];
    for(m=0;m<8;m++){
        for(n=0;n<8;n++){
            sprintf(buff,"ledary(%d,%d)",m,n);
            Tcl_LinkVar(interp, buff, (char *)(&(vled[m][n])), TCL_LINK_UCHAR);
        }
    }


    
    /* tclコマンド */
    static char command[] =
        "bind . <KeyPress-z> {set z 1} \n"
        "bind . <KeyRelease-z> {set z 0} \n"
        "bind . <KeyPress-x> {set x 1} \n"
        "bind . <KeyRelease-x> {set x 0} \n"
        "wm title . \"Forcy USB Simulator\" \n"
        "wm protocol . WM_DELETE_WINDOW exit \n"
        "wm resizable . 0 0\n"
        "canvas .c0 -width 240 -height 240 -background black\n"
        "pack .c0 \n"
        "for {set m 0} {$m < 8} {incr m} { \n"
        "for {set n 0} {$n < 8} {incr n} { \n"
        "set ledary($n,$m) 1 \n"
        "set ary($n,$m) [.c0 create oval [expr 30*$m+5] [expr 30*$n+5] [expr 30*$m+30] [expr 30*$n+30] ] \n"
        "}} \n"
        "proc rpt {} {"
        "   global zo xo ary ledary\n"
//        "   puts \"$zo $xo\" \n"
        "   for {set m 0} {$m < 8} {incr m} { \n"
        "   for {set n 0} {$n < 8} {incr n} { \n"
        "     if { $ledary($m,$n) == 1 } { set col red } else { set col gray } \n"
        "     .c0 itemconfigure $ary($m,$n) -fill $col \n"
        "   }} \n"
        "   after 10 rpt } \n"
        "rpt \n"
        ;
     
    /* スクリプト実行 */
    Tcl_Eval(interp, command);

    /* Tkのメインループ開始 */
    Tk_MainLoop();
}
 
/***********************************
 * オートリピートキャンセラー
 ***********************************/
static int ar_cancel(struct SW *sw)
{
    int in=sw->in;
    int s=sw->s;
    int cnt=sw->cnt;
    int out=sw->out;
    switch(s){
    case 0:
        if(in==1){
            s=1;
            out=1;
            break;
        }
        out=0;
        break;
    case 1:
        if(in==0){
            cnt=10;
            s=2;
            break;
        }
        if((0<cnt)&&(in==1)){
            s=1;
            break;
        }
        break;
    case 2:
        if(cnt==0){
            s=0;
            break;
        }
        cnt--;
        break;
    }
    sw->s=s;
    sw->cnt=cnt;
    sw->out=out;
    return out;
}

/******************************************************************
	スイッチ読み取り
******************************************************************/

void  key_check(){
    sw=0;
    if(sw_x.out)
        sw |=1;
    if(sw_z.out)
        sw |=2;
}


/******************************************************************
	マトリクスＬＥＤ表示
******************************************************************/

static void view_update()
{
	int			x, y, k;
	for( y=0; y<LED_SZ; y++) {
		for( x=0; x<LED_SZ; x++){
            vled[7-y][x]=(led[y] >> x )& 1;
        }
    }
}
    
/******************************************************************
	ビープ音
******************************************************************/

void _sound( unsigned char tone )
{
	if(tone==BEEP_HIGH)
		system("/usr/bin/beep -f 1600 -l 100");
	else
		system("/usr/bin/beep -f 400 -l 100");
}


/******************************************************************
	ＣＯＭポート送受信
******************************************************************/

static int uart_getc( void )
{
	int			data;

	if( pc_rptr==pc_wptr )
		return -1;

	data	= (short)pc_data[pc_rptr++];
	pc_rptr	&= PCBUFSZ-1;
	return data;
}

void uart_transmit( unsigned char data )
{
	int			next;

	next	= (pc_wptr+1) & (PCBUFSZ-1);
	if( pc_rptr!=next ) {
		pc_data[pc_wptr]	= data;
		pc_wptr	= next;
	}
}

void _reset(){
    exit(1);
}

