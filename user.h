#define LED_SZ 8
typedef unsigned char uchar;
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
