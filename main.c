/*
	main.c : ForCy-USB シミュレータ

	Copyright(C) 2011 Osamu Tamura @ Recursion Co., Ltd.
					All rights reserved.
*/

// UNIX Version: Daisuke Takago 2014

#include<curses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include"user.h"

#define	SCRN_LEFT	24
#define	SCRN_TOP	3

static void			view_update();
void key_check( void );
static int			uart_getc( void );


#define	PCBUFSZ		64
static int				pc_rptr;
static int				pc_wptr;
static unsigned char	pc_data[PCBUFSZ];

int main(){

	initscr();
    keypad(stdscr, true);
    wtimeout(stdscr, 100);
	noecho();
	curs_set(0); 

	user_init();
	
	while(1) {
		key_check();
		user_main();
		view_update();
		usleep( 100 );
	}

	endwin();
	return 0;	
}


/******************************************************************
	マトリクスＬＥＤ表示
******************************************************************/

static void view_update()
{
	int X,Y; // pos
	int			x, y, m;

	Y	= SCRN_TOP + LED_SZ;
	for( y=0; y<LED_SZ; y++,Y-- ) {
		X	= SCRN_LEFT;
		for( x=0,m=0x80; x<LED_SZ; x++,m>>=1, X+=2 ) {
			move(Y,X);
			if( led[y]&m )
				printw("o");
			else
				printw("-");
		}
	}
	refresh();
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
	スイッチ読み取り
******************************************************************/

void key_check( void )
{
	sw	= 0;
	int		c=0;
	while( c!=-1 ) {
		c	= getch();
		if( c=='z' )
			sw	|= 1;
		if( c=='x' )
			sw	|= 2;
	}
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
