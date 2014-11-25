#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "common.h"

#define SW ((~PINC >> 4) & 3)
#define SW_COUNT 30
#define TIMER_1SEC_MAX 10

#define ADCINIT (_BV(ADEN) | 0b110)
#define ADCSTART (_BV(ADSC) | _BV(ADIF))
#define ADCBUSY ((ADCSRA & _BV(ADIF)) == 0)
void setSeed();

static volatile uchar scan; // led走査
static volatile uchar clk;  // 間引き
static volatile uchar timer_1sec_counter = 0;
static volatile uchar timer_1sec_count_f = FALSE;
static uchar tones[5][11] = {
    {476, 449, 424, 400, 378, 356, 336, 317, 299, 283, 252},
    {237, 224, 211, 199, 188, 177, 167, 158, 149, 141, 125},
    {118, 111, 105, 99, 93, 88, 83, 78, 74, 70, 62},
    {58, 55, 52, 49, 46, 43, 41, 38, 36, 34, 30},
    {28, 27, 25, 24, 22, 21, 20, 18, 17, 16, 14},
};
int seed;
volatile uchar wait = 0;    // スイッチ変化時の待ち
volatile uchar pre_sw;

char s[100];

void sw_update();
static void buzzer(uchar tone, uchar len);
static uchar parseInt(char* s, uchar pos, uchar* res);

ISR(PCINT1_vect){
    if(wait == 0){
        wait = 30;
    }
    // ブロックしているピン変化割り込み要求をキャンセル
    PCIFR |= _BV(PCIF1);
}

ISR(TIMER0_COMPA_vect)
{
    unsigned char sc;
    // 2mSごとにLED走査
    PORTB = 0;
    scan = (scan + 1) & 7;
    sc = ~(1 << scan);
    PORTC = 0x30 | (sc & 0x0f); // sw1,2をプルアップ
    PORTD = (sc & 0xf0) | 0x08;   // ブザーを有効
    PORTB = led[scan];
    if (++clk >= 50) {          // 100mSごとに起動
        clk = 0;
        user_main();
        if(timer_1sec_count_f){
            timer_1sec_counter ++;
        }
    }
}

/*
ISR(TIMER1_COMPA_vect){
    timer_1sec_comp();
}
*/

ISR(TIMER1_COMPA_vect){
    flash = flash ? 0 : 1;
}


// 1文字出力
void _putc(char c){
    if(c == '\n')
        _putc('\r');
        while((UCSR0A & (1 << UDRE0)) == 0)
            wdt_reset();
        UDR0 = c;
}
void _puts(char *s){
    unsigned char n=0;
    while(s[n] != '\0'){
        _putc(s[n]);
        n++;
    }
}

int main(void)
{
    /* ポート設定 */
    DDRB = 0xff;
    DDRC = 0x0f;
    DDRD = 0xfa;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x08;
    // ポートCのピン変化割り込みを有効
    PCICR = _BV(PCIE1);
    // 割り込みを認めるビット位置を指定する
    PCMSK1 = _BV(PCINT12) | _BV(PCINT13);
    //  タイマ・カウンタ設定
    // タイマ割り込み設定
    OCR0A = 249;                // 2mS
    TCCR0A = 2;
    TCCR0B = 3;                 // 1/64
    TIMSK0 |= (1 << OCIE0A);    // 割り込み設定
    // LED点滅用タイマ
    OCR1A = 20;                 // LED
    TCCR1A = 0x02;              // CTC COMA,B
    TCCR1B = 0x02;              // 1/64
    TIMSK1 |= (1 << OCIE1A);
    // サウンド
    TCCR2A = 0x12;
    //TCCR2B = 0x44;              // CTC 1/64
    // A/D
    ADMUX = 0x45;
    ADCSRA = (_BV(ADEN) | 0b110);

    wdt_enable(WDTO_2S);

    UCSR0B = 0;
    UCSR0C = 0x06;
    UBRR0 = 103;
    UCSR0B = _BV(TXEN0);

    // ゲーム初期化
    setSeed();
    pre_sw = sw = SW;
    user_init();
    sei();
    for (;;) {
        wdt_reset();
        sw_update();
        if(TIMER_1SEC_MAX <= timer_1sec_counter){
            timer_1sec_comp();
            timer_1sec_counter = 0;
        }
        if(gameover){
            user_init();
        }
    }
    return 0;
}



void timer_1sec_start(){
    //TCCR1B |= 0x05;
    timer_1sec_counter = 0;
    timer_1sec_count_f = TRUE;
}

void timer_1sec_stop(){
    //TCCR1B &= 0xf8;
    //TCNT1 = 0x0000;
    timer_1sec_count_f = FALSE;
    timer_1sec_counter = 0;
}

void setSeed(){
    uchar ddrc = DDRC;

    PORTB = 0;
    DIDR0 = _BV(ADC5D); // デジタル入力無効
    DDRC |= 0x20;
    DDRC &= ~0x20;
    _delay_ms(100);

    ADCSRA |= (_BV(ADSC) | _BV(ADIF));    // ADCスタート
    while((ADCSRA & _BV(ADIF)) == 0){
        wdt_reset();
    }

    seed = ADC;
    srand(seed);
    DDRC = ddrc;
    DIDR0 &= ~_BV(ADC5D);   // デジタル入力有効
}

void sw_update(){
    // スイッチ変化時に30ms待ってからもう1回読み取る
    if(wait > 0){
        if(--wait == 0){
            sw = SW;
        }
        _delay_ms(1);
    }
}

void _wdt_reset(){
    wdt_reset();
}

void melody(char *score){
    uchar i, t=4, oct=4, len=200;
    uchar bu;

    for(i=0; score[i] != '\0'; i++){
        if('a' <= score[i] && score[i] <= 'g'){
            switch(score[i]){
                case 'c': t = 0; break;
                case 'd': t = 2; break;
                case 'e': t = 4; break;
                case 'f': t = 5; break;
                case 'g': t = 7; break;
                case 'a': t = 9; break;
                case 'b': t = 11; break;
            }
        }else{
            if('1' <= score[i] && score[i] <= '9'){
                i += parseInt(score, i, &bu);
                len /= bu;
            }else{
                switch(score[i]){
                    case '#': t++; break;
                    case '+': t++; break;
                    case '-': t--; break;
                    case '<': oct++; break;
                    case '>': oct--; break;
                    case '.':
                              i += parseInt(score, i, &bu);
                              len /= bu;
                              break;
                    defult: i++; break;
                }
            }
        }
        // 鳴らす
        if(('a' <= score[i+1] && score[i+1] <= 'g') || score[i+1] == '\0'){
            buzzer(tones[oct-3][t], len);
            _wdt_reset();
            t = 0; oct = 4; len = 200;
        }
    }
}

// ブザー開始
static void buzzer(uchar tone, uchar len){
    TCNT2 = 0;
    OCR2A = tone;
    TCCR2B |= 0x04;
    _delay_ms(100);
    TCCR2B &= 0xf8;
}

static uchar parseInt(char* s, uchar pos, uchar* res){
    uchar i, j;
    uchar tmp;
    uchar n=1;        // 第n位
    char tmps[2];
    *res = 0;

    for(i=0; ; i++){
        tmps[0] = s[pos+i];
        tmps[1] = '\0';
        tmp = atoi(tmps);
        if(tmp == 0) break;
        for(j=1; j<n; j++){
            (*res) *= 10;
        }
        (*res) += tmp;
        n++;
    }

    return n;
}

