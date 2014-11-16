#define LED_SZ 8        // 8x8LED
#define FIELD_SZ 32     // ゲームフィールドサイズ
// ダンジョンの状態
#define P 0     // 通路
#define W 1     // 壁
typedef unsigned char uchar;
typedef unsigned long ulong;
enum
{
    BEEP_LOW,
    BEEP_HIGH
};
extern void _sound(uchar tone);
extern volatile uchar sw;
extern volatile uchar led[LED_SZ];
extern volatile uchar gameover;
extern void user_init(void);
extern void user_main(void);
