#include "user.h"

// ローカル関数
static void MoveEnemy(void);
static void MoveBullet(void);
static void MoveFort(void);
static void UpdateLED(void);
// グローバル変数
volatile uchar sw = 0;      // 押しボタン
volatile uchar led[LED_SZ]; // マトリクスLED
volatile uchar count = 0;   // 撃墜数
volatile uchar gameover = 0;// ゲーム終了フラグ
// ローカル変数
static uchar enemy[LED_SZ]; // 敵の位置
static uchar bullet[LED_SZ];// 弾の位置
static uchar fort = 3;      // 放題の中心位置
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    uchar i;
    for (i = 0; i < LED_SZ; i++) {
        enemy[i] = 0;
        bullet[i] = 0;
    }
    fort = 3;
    count = 0;
    gameover = 0;
}
/*
    ユーザ処理
 */
void user_main(void)
{
    MoveEnemy();
    MoveBullet();
    MoveFort();
    UpdateLED();
}
/*
    敵の移動
 */
static void MoveEnemy(void)
{
    static uchar tick = 0;
    static uchar dir = 0;
    uchar i;
    if (++tick < 10)
        return;
    tick = 0;
    switch (dir) {
        case 0:
            for (i = 0; i < LED_SZ; i++)
                enemy[i] >>= 1;
            break;
        case 2:
            for (i = 0; i < LED_SZ; i++)
                enemy[i] <<= 1;
            break;
        default:
            if (enemy[0])
                _sound(BEEP_LOW);
            // 下へ
            for (i = 0; i < (LED_SZ - 1); i++)
                enemy[i] = enemy[i + 1];
            enemy[LED_SZ - 1] = dir == 3 ? 0xaa :
                0;
            break;
    }
    dir = (dir + 1) & 3;
}
/*
    弾の移動
 */
static void MoveBullet(void)
{
    uchar i;
    // 上へ
    for (i = LED_SZ - 1; i; i--)
        bullet[i] = bullet[i - 1];
    bullet[0] = 0;
}

/*
    砲台
 */
static void MoveFort(void)
{
    switch (sw) {
        case 1:     // 左へ
            if (fort > 1)
                fort--;
            break;
        case 2:     // 右へ
            if (fort < (LED_SZ - 2))
                fort++;
            break;
        case 3:     // 弾の発射
            bullet[0] |= 0x80 >> fort;
            break;
    }
}
/*
    LED表示の更新
 */
static void UpdateLED(void)
{
    uchar i, j;
    // 敵と弾
    for (i = 0; i < LED_SZ; i++) {
        j = enemy[i] & bullet[i];
        if (j) {
            enemy[i] ^= j;
            bullet[i] ^= j;
            _sound(BEEP_HIGH);
            count ++;
            if(count >= 10){
                gameover = 1;
            }
        }
        led[i] = enemy[i] | bullet[i];
    }
    // 砲台
    led[0] |= (uchar)(0x1c0 >> fort);
    led[1] |= (uchar)(0x80 >> fort);
}
