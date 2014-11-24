#define LED_SZ 8        // 8x8LED
#define LED_MAX LED_SZ-1
#define FIELD_SZ 16     // ゲームフィールドサイズ
#define FIELD_MAX FIELD_SZ-1
// ダンジョンの状態 オブジェクトID
#define ID_PASSAGE  0     // 通路
#define ID_WALL     1     // 壁
#define ID_PLAYER   2
#define ID_MOB      3
#define ID_BOMB     4
// 方向の定義
#define DIR_RIGHT   0
#define DIR_UP      1
#define DIR_LEFT    2
#define DIR_DOWN    3
// プレイヤーの定義
#define PLAYER_MAX_HP 5
#define PLAYER_ATTACK 1
// マーカーの定義
#define MARKER_SHOW 1
#define MARKER_HIDE 0
// 爆弾の定義
#define BOMB_TIMELIMIT 5        // [sec]
#define BOMB_ATACK 2
// 敵キャラの定義
#define MOB_IDLE 0
#define MOB_ACTIVE 1
#define MOB_ATACK 1
#define MOB_HP 3
// 雑多定義
#define MOVED 1
#define UNMOVE 0
typedef unsigned char uchar;    // 1byte
typedef unsigned int  uint;     // 2byte
//typedef unsigned long ulong;    // 4byte
enum
{
    BEEP_LOW,
    BEEP_HIGH
};
extern void _sound(uchar tone);
extern volatile uchar sw;
extern volatile uchar led[LED_SZ];
extern volatile uchar gameover;
extern volatile uchar flash;
extern void user_init(void);
extern void user_main(void);
extern void timer_1sec_comp(void);
extern void timer_1sec_start(void);
extern void timer_1sec_stop(void);


