/**************************************************************************
    Copyright(c) 2014 KarasawaTakahiro, Kanazawa Institute of Technology

    ローグライクゲーム
    3EP1-12 唐澤貴大 金沢工業大学 工学部 情報工学科
    ver 0.1, 2014/11/26
    rogue-like.h
**************************************************************************/

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

// ローカル変数
static volatile uchar scan; // led走査
static volatile uchar clk;  // 間引き
static volatile uchar timer_1sec_counter = 0;       // 1秒をカウントするための変数
static volatile uchar timer_1sec_count_f = FALSE;   // カウント中かのフラグ
static volatile uchar buzzer_counter = FALSE;       // ブザーを鳴らすフラグ兼カウンタ
static volatile uchar wait = 0;    // スイッチ変化時の待ち
static volatile uchar pre_sw;      // チャタリング用の変数
// グローバル変数
char s[100];
int seed;
// ローカル関数
static void buzzer(uchar tone);     // ブザーを鳴らす
static void stopBuzzer(void);       // ブザーを止める
static void sw_update();                   // スイッチを更新
// グローバル関数
void setSeed();                     // シード値設定

// SW割り込み
ISR(PCINT1_vect){
    if(wait == 0){
        wait = 30;
    }
    // ブロックしているピン変化割り込み要求をキャンセル
    PCIFR |= _BV(PCIF1);
}

// システムタイマー割り込み
ISR(TIMER0_COMPA_vect)
{
    unsigned char sc;
    // 2mSごとにLED走査
    PORTB = 0;
    scan = (scan + 1) & 7;
    sc = ~(1 << scan);
    PORTC = 0x30 | (sc & 0x0f);     // sw1,2をプルアップ
    PORTD = (sc & 0xf0) | 0x08;     // ブザーを有効
    PORTB = led[scan];
    if (++clk >= 50) {              // 100mSごとに起動
        clk = 0;
        user_main();                // ゲームのメイン関数
        if(timer_1sec_count_f)      // 1秒カウンタの処理
            timer_1sec_counter ++;
        if(0 < buzzer_counter)      // ブザーの処理
            stopBuzzer();
    }
}

// 8x8LEDの点滅のためのタイマーの割り込み
ISR(TIMER1_COMPA_vect){
    flash = flash ? 0 : 1;          // 明暗を反転
}


// 1文字出力
void _putc(char c){
    if(c == '\n')
        _putc('\r');
        while((UCSR0A & (1 << UDRE0)) == 0)
            wdt_reset();
        UDR0 = c;
}

// 文字列をシリアルモニタに出力
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
    TCCR2A = 0x12;              // CTC 1/64
    // A/D
    ADMUX = 0x45;
    ADCSRA = (_BV(ADEN) | 0b110);
    // WDT
    wdt_enable(WDTO_2S);
    // シリアル通信設定
    UCSR0B = 0;
    UCSR0C = 0x06;
    UBRR0 = 103;
    UCSR0B = _BV(TXEN0);

    // システム初期化
    sei();                      // 割り込み有効化
    setSeed();                  // シード値
    pre_sw = sw = SW;           // 押しっぱなし対策

    // ゲーム開始
    user_init();
    se(SE_TYPE_HIGH); se(SE_TYPE_HIGH);     // 開始音
    for (;;) {
        wdt_reset();
        sw_update();
        // 1秒タイマ処理
        if(TIMER_1SEC_MAX <= timer_1sec_counter){       // 1秒経過
            timer_1sec_comp();
            timer_1sec_counter = 0;
        }
        // ゲームオーバー処理
        if(gameover){                       // ケームオーバー処理
            se(SE_TYPE_LOW); se(SE_TYPE_BASIC);
            user_init();        // ゲームをはじめから
        }
    }

    return 0;
}

// 1秒タイマー開始
void timer_1sec_start(){
    timer_1sec_counter = 0;
    timer_1sec_count_f = TRUE;
}

// 1秒タイマ停止
void timer_1sec_stop(){
    timer_1sec_count_f = FALSE;
    timer_1sec_counter = 0;
}

// シード値を設定
void setSeed(){
    uchar ddrc = DDRC;      // 設定を保存

    PORTB = 0;
    DIDR0 = _BV(ADC5D); // デジタル入力無効
    DDRC |= 0x20;       // 放電
    DDRC &= ~0x20;
    _delay_ms(100);

    ADCSRA |= (_BV(ADSC) | _BV(ADIF));    // ADCスタート
    while((ADCSRA & _BV(ADIF)) == 0){
        wdt_reset();
    }

    // シード値設定
    seed = ADC;
    srand(seed);

    DDRC = ddrc;            // 設定を戻す
    DIDR0 &= ~_BV(ADC5D);   // デジタル入力有効
}

// SWの値を更新
void sw_update(){
    // スイッチ変化時に30ms待ってからもう1回読み取る
    if(wait > 0){
        if(--wait == 0){
            sw = SW;
        }
        _delay_ms(1);
    }
}

// ウォッチドッグタイマをリセットする
void _wdt_reset(){
    wdt_reset();
}


// ブザー開始
static void buzzer(uchar tone){
    TCNT2 = 0;
    OCR2A = tone;
    TCCR2B |= 0x04;
    buzzer_counter = TRUE;
}

// ブザーを止める
static void stopBuzzer(){
    // ブザーを鳴らしたら
    if(BZ_LENGTH < buzzer_counter){
        // 止める
        TCCR2B &= 0xf8;
        buzzer_counter = FALSE;
    }else{
        // 時間経過をカウント
        buzzer_counter ++;
    }
}

// 効果音を鳴らす
void se(uchar pattern){
    // 効果音のタイプで判別
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

