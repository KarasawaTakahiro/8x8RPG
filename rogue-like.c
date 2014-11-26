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
int seed;
volatile uchar wait = 0;    // スイッチ変化時の待ち
volatile uchar pre_sw;

char s[100];

void sw_update();
static void buzzer(uchar tone);

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
    sei();
    setSeed();
    pre_sw = sw = SW;
    user_init();
    se(SE_TYPE_HIGH); se(SE_TYPE_HIGH);     // 開始音
    for (;;) {
        wdt_reset();
        sw_update();
        if(TIMER_1SEC_MAX <= timer_1sec_counter){
            timer_1sec_comp();
            timer_1sec_counter = 0;
        }
        if(gameover){
            se(SE_TYPE_LOW); se(SE_TYPE_BASIC);
            se(SE_TYPE_LOW); se(SE_TYPE_BASIC);
            se(SE_TYPE_LOW); se(SE_TYPE_BASIC);
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


// ブザー開始
static void buzzer(uchar tone){
    TCNT2 = 0;
    OCR2A = tone;
    TCCR2B |= 0x04;
    _delay_ms(100);
    TCCR2B &= 0xf8;
}

// 効果音を鳴らす
void se(uchar pattern){
    switch(pattern){
        case SE_TYPE_BASIC:
            buzzer(SE_BASIC);
            break;
        case SE_TYPE_HIGH:
            buzzer(SE_HIGH);
            break;
        case SE_TYPE_LOW:
            buzzer(SE_LOW);
            break;
        case SE_TYPE_MIDLE:
            buzzer(SE_MIDLE);
            break;
    }
}
