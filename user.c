#include "user.h"


// 構造体
typedef struct player_s{
    uchar dir;
    uchar x;
    uchar y;
    uchar hp;
    uchar obj_id;
}player_t;

typedef struct mob_s{
    uchar active;
    uchar attack;
    uchar x;
    uchar y;
    uchar hp;
    uchar dir;
    uchar obj_id;
} mob_t;

typedef struct bomb_s{
    uchar x;
    uchar y;
    uchar timelimit;
    uchar attack;
    uchar obj_id;
    uchar set;      // 設置済みか
} bomb_t;
// ローカル関数
static void MoveEnemy(void);
static void MoveBullet(void);
static void MoveFort(void);
static void UpdateLED(void);
// グローバル変数
volatile uchar sw = 0;      // 押しボタン
volatile uchar led[LED_SZ]; // マトリクスLED
volatile uchar gameover = 0;// ゲーム終了フラグ
volatile uchar flash = 0;
// ローカル変数
volatile uint field[FIELD_SZ] = {0};
volatile uchar obj_tbl[FIELD_SZ][FIELD_SZ] = {{0}};
volatile player_t player;
volatile uchar marker_f;
volatile bomb_t bomb;
mob_t mob;
volatile uchar playerMove_f;

volatile uchar print = 0x00;
volatile uchar print1 = 0x00;
volatile uchar print2 = 0x00;

// プロトタイプ
void initField(void);
void changeDirection(void);
uchar searchFront(uchar, uchar, uchar);
void walk(void);
void showMarker(void);
void showDungeon(void);
void getFrontCoord(uchar, uchar, uchar, uchar*, uchar*);
void setObject(uchar, uchar, uchar);
void setBomb(void);
void convObjToField(void);
void bornMob(uchar, uchar);
void initPlayer(uchar, uchar);
void damage(uchar, uchar, uchar);
void hitPassage(uchar, uchar, uchar);
void hitMob(uchar, uchar, uchar);
void hitPlayer(uchar);
void deadMob(uchar, uchar);
void initBomb(void);
void mobMove(mob_t*);
void clearObjTbl(void);
void mvObject(uchar src_x, uchar src_y, uchar dist_x, uchar dist_y, uchar obj_id);

/*
    フレームワークから呼ばれる関数群
*/
/*
    ユーザ処理の初期化
 */
void user_init(void)
{
    gameover = 0;


    bomb.obj_id = ID_BOMB;


    clearObjTbl();
    initField();
    initPlayer(2, 2);
    playerMove_f = UNMOVE;
    bornMob(4, 3);
}
/*
    ユーザ処理
 */
void user_main(void)
{
    playerMove_f = UNMOVE;   // プレイヤーの行動フラグをリセット
    MoveEnemy();
    MoveBullet();
    MoveFort();
    if(playerMove_f == MOVED)
        mobMove(&mob);
    UpdateLED();
}
/*
    敵の移動
 */
static void MoveEnemy(void)
{
}
/*
    弾の移動
 */
static void MoveBullet(void)
{
}

/*
    プレイヤーの行動
 */
static void MoveFort(void) {
    switch(sw){
        case 1:
            if(searchFront(player.x, player.y, player.dir) == ID_PASSAGE)
                walk();
            playerMove_f = MOVED;
            break;
        case 2:
            changeDirection();
            break;
        case 3:
            setBomb();
            playerMove_f = MOVED;
            break;
    }
}
/*
    LED表示の更新
 */
static void UpdateLED(void)
{
    showDungeon();
    led[3] |= 0x10; // プレイヤーの位置
    // 方向
    showMarker();

    //led[0] = obj_tbl[player.y][player.x];//(player.x << 4) | player.y;
    //led[1] = (mob.x << 4) | mob.y;
    //led[6] = print2;//mob.dir;
    led[7] = player.hp;
    //print2 = 0;
}

/*
    オリジナル関数群
*/
// フィールドの初期化
void initField(){
    uchar x, y;

    for(y=0; y<FIELD_SZ; y++){
        for(x=0; x<FIELD_SZ; x++){
            if(y == 0 || y == FIELD_MAX)
                obj_tbl[y][x] = ID_WALL;
            else
                if(x == 0 || x == FIELD_MAX)
                    obj_tbl[y][x] = ID_WALL;
                else
                    obj_tbl[y][x] == ID_PASSAGE;
        }
    }
}

// オブジェクトテーブルをフィールドに変換
void convObjToField(){
    uchar x, y;
    uint row;       // フィールドの任意の行

    // オブジェクトテーブルを参照していく
    for(y=0; y<FIELD_SZ; y++){                  // テーブルから行を取り出す
        row = 0x0000;                           // フィールドの１行
        for(x=0; x<FIELD_SZ; x++){              // 各行の値を参照
            row <<= 1;                          // 列送り
            if(obj_tbl[y][x] == ID_MOB)         // MOBが存在したら
                row |= (uint)flash;
            else if(obj_tbl[y][x] != ID_PASSAGE)     // オブジェクトが存在したら
                row ++;                         // フィールドに反映
        }
        field[y] = row;                         // 行に当てはめる
    }
}

// プレイヤーの移動方向を変更
void changeDirection(){
    player.dir = (player.dir + 1) % 4;
    marker_f = MARKER_SHOW;
}

// 前方のオブジェクトIDを返す
uchar searchFront(uchar x, uchar y, uchar dir){
    uchar fx, fy;

    getFrontCoord(x, y, dir, &fx, &fy);

    return obj_tbl[fy][fx];
}

/*
    プレイヤーの移動
    directionに従ってプレイヤーの座標を更新する
*/
void walk(){
    uchar x = player.x;
    uchar y = player.y;

    switch(player.dir){
        case 0: player.x++;    // 右
            break;
        case 1: player.y++;    // 上
            break;
        case 2: player.x--;    // 左
            break;
        case 3: player.y--;    // 下
            break;
    }
    mvObject(x, y, player.x, player.y, player.obj_id);
    marker_f = MARKER_HIDE;
}

// プレイヤーの進行方向を示すマーカーを表示する
void showMarker(){
    if(marker_f == MARKER_SHOW){
        switch(player.dir){
        case 0: led[3] |= 0x08;    // 右
            break;                      
        case 1: led[4] |= 0x10;    // 上
            break;                      
        case 2: led[3] |= 0x20;    // 左
            break;                      
        case 3: led[2] |= 0x10;    // 下
            break;
        }
        //led[7] = player.dir;
    }
}

// ダンジョンの表示
void showDungeon(){
    uchar i;
    uint mask;    // マスク
    signed char curx;  // X方向の基準点
    signed char cury;  // Y方向の基準点

    convObjToField();

    for(i=0; i<LED_SZ; i++){
        led[i] = 0x00;      // 一度真っ白にする

        // フィールドの表示と，左右フィールド外の表示
        cury = player.y - 3;  // 画面の下を基準に
        if(player.x-3 < 0){                                     // 画面の左端がfieldの左外
            curx = player.x - 3;                                // 画面の左端を基準にする
            mask = (0xff00 << (-curx));
            led[i] = (uchar)((field[cury+i] & mask) >> (FIELD_SZ - LED_SZ - curx));    // fieldから必要分を切り取って，右に詰める

        }else if(0 <= player.x-3 && player.x+4 <= FIELD_MAX){   // 真ん中 フィールドの表示
            curx = player.x - 3;                                // 画面の左端を基準に
            mask = (0xff00 >> curx);                        // マスクの調整
            led[i] = (uchar)(((field[cury+i] & mask)) >> (FIELD_SZ - (LED_SZ + curx))); // fieldから必要分を切り取って，右に詰める

        }else if(FIELD_MAX < player.x+4){                       // 画面の右端がFIELD_MAXより右
            curx = player.x + 4;                                // 画面の右端を基準にする
            mask = (0x00ff >> (curx - FIELD_MAX + 2));      // マスクの調整
            led[i] = (uchar)((field[cury+i] & mask) << (curx - FIELD_MAX + 2)); // fieldから必要分を切り取って，左に寄せる
        }

        // フィールド外の上下表示
        if(FIELD_MAX < player.y+4){                 // 上
            cury = player.y + 4;                    // 画面の上端を基準に
            if((LED_MAX-1)-(cury-FIELD_MAX) <= i){  // フィールド外
                led[i] = 0x0000;
            }
        }else if(player.y-3 < 0){                   // 下
            cury = player.y - 3;                    // 画面の下端を基準に
            if(cury < 0 && i < (-cury)){            // フィールド外
                led[i] = 0x0000;
            }
        }
    }

}

/*
    オブジェクトの前方の座標を取得する
    x, y : オブジェクトの座標
    dir  : オブジェクトの方向
    fx, fy : 戻り用ポインタ
*/
void getFrontCoord(uchar x, uchar y, uchar dir, uchar* fx, uchar* fy){
    switch(dir){
        case 0:     // 右の値を返す
            *fx = x+1;  *fy = y;
            break;
        case 1:     // 上
            *fx = x;  *fy = y+1;
            break;
        case 2:     // 左
            *fx = x-1;  *fy = y;
            break;
        case 3:     // 下
            *fx = x;  *fy = y-1;
            break;
    }
}

// オブジェクトテーブルにオブジェクトを登録する
void setObject(uchar x, uchar y, uchar obj_id){
    obj_tbl[y][x] = obj_id;
}

// オブジェクトテーブルの座標から指定したオブジェクトを削除する
uchar rmObject(uchar x, uchar y, uchar obj_id){
    if(obj_tbl[y][x] == obj_id){
        obj_tbl[y][x] = ID_PASSAGE;
        return 1;
    }else{
        return 0;
    }
}

// 爆弾を置く
void setBomb(){
    uchar fx, fy;

    // 爆弾を未設置 && 前方の確認
    if(bomb.set == 0 && searchFront(player.x, player.y, player.dir) == ID_PASSAGE){   
        getFrontCoord(player.x, player.y, player.dir, &fx, &fy);
        bomb.timelimit = BOMB_TIMELIMIT;    // タイムリミットをセット
        setObject(fx, fy, bomb.obj_id);     // 爆弾を設置
        bomb.x = fx;
        bomb.y = fy;
        bomb.set = 1;
        timer_1sec_start();
    }
}

// 爆弾が爆発
void explodeBomb(){
    signed char i, j, x, y;

    // 消滅処理
    bomb.set = 0;
    rmObject(bomb.x, bomb.y, bomb.obj_id);

    // ダメージ処理
    for(i=-1; i<=1; i++){
        for(j=-1; j<=1; j++){
            x = bomb.x + j;
            y = bomb.y + i;
            damage(x, y, bomb.attack);
        }
    }
}

void timer_1sec_comp(){
    if(bomb.set){
        print = bomb.timelimit;
        if(bomb.timelimit){
            bomb.timelimit --;
        }else{
            explodeBomb();
            timer_1sec_stop();
        }
    }
}

// 敵の設置
void bornMob(uchar x, uchar y){
    mob.active = MOB_IDLE;
    mob.attack = MOB_ATACK;
    mob.x = x;
    mob.y = y;
    mob.hp = MOB_HP;
    mob.dir = DIR_UP;
    mob.obj_id = ID_MOB;

    setObject(x, y, mob.obj_id);
}

void initPlayer(uchar x, uchar y){
    player.x = x;
    player.y = y;
    player.dir = DIR_RIGHT;
    player.hp = PLAYER_MAX_HP;
    player.obj_id = ID_PLAYER;
    setObject(player.x, player.y, player.obj_id);

    marker_f = MARKER_SHOW;

    initBomb();
}

// 任意の座標に任意のダメージを与える
void damage(uchar x, uchar y, uchar value){
    switch(obj_tbl[y][x]){
        case ID_PASSAGE:
            hitPassage(x, y, value);
            break;
        case ID_MOB:
            hitMob(x, y, value);
            break;
        case ID_PLAYER:
            hitPlayer(value);
            break;
    }
}

// 壁へのダメージ
void hitPassage(uchar x, uchar y, uchar val){
    // 外壁以外は消滅
    if((0 <  x) && (x < FIELD_MAX) && (0 < y) && (y < FIELD_MAX)){
        rmObject(x, y, ID_PASSAGE);
    }
}

// Mobへのダメージ
void hitMob(uchar x, uchar y, uchar val){
    if(mob.hp < val){
        mob.hp = 0;
        deadMob(x, y);
    }else{
        mob.hp -= val;
    }
}

// プレイヤーへのダメージ
void hitPlayer(uchar val){
    if(player.hp < val){
        player.hp = 0;
        gameover = 1;
    }else{
        player.hp -= val;
    }
}

// MOBの死亡処理
void deadMob(uchar x, uchar y){
    rmObject(x, y, ID_MOB);
}

void initBomb(){
    bomb.timelimit = BOMB_TIMELIMIT;
    bomb.attack = BOMB_ATACK;
    bomb.obj_id = ID_BOMB;
    bomb.set = 0;
}

// オブジェクトを移動する
void mvObject(uchar src_x, uchar src_y, uchar dist_x, uchar dist_y, uchar obj_id){
    if(rmObject(src_x, src_y, obj_id))
        setObject(dist_x, dist_y, obj_id);
}

// MOBの攻撃
void mobAttack(mob_t m){
    uchar fx, fy;

    getFrontCoord(m.x, m.y, m.dir, &fx, &fy);
    damage(fx, fy, m.attack);
}

// mobの進行方向を決める
void mobChangeDirection(mob_t* m){
    signed char dx = (*m).x - player.x;
    signed char dy = (*m).y - player.y;

    if(dx < 0){
        dx *= (-1);
        if(dy < 0){
            dy *= (-1);
            if(dx < dy) m->dir = DIR_UP;
            else if(dy <= dx) m->dir = DIR_RIGHT;
        }else if(0 <= dy){
            if(dx < dy) m->dir = DIR_DOWN;
            else if(dy <= dx) m->dir = DIR_RIGHT;
        }
    }else if(0 <= dx){
        if(dy < 0){
            dy *= (-1);
            if(dx < dy) m->dir = DIR_UP;
            else if(dy <= dx) m->dir = DIR_LEFT;
        }else if(0 <= dy){
            if(dx < dy) m->dir = DIR_DOWN;
            else if(dy <= dx) m->dir = DIR_LEFT;
        }
    }
}

// MOBの移動
void mobMove(mob_t *m){
    uchar x, y;
    uchar front;

    mobChangeDirection(m);

    print2 = (m->dir << 4);
    print2 |= searchFront(m->x, m->y, m->dir);

    front = searchFront(m->x, m->y, m->dir);
    if(front == ID_PASSAGE){
        x = m->x;
        y = m->y;

        switch(m->dir){
            case DIR_RIGHT: m->x++;  // 右
                            break;
            case DIR_UP: (m->y)++;     // 上
                         print1 = m->y;
                         break;
            case DIR_LEFT: m->x--;   // 左
                           break;
            case DIR_DOWN: m->y--;   // 下
                           break;
        }

        mvObject(x, y, m->x, m->y, m->obj_id);
    }else if(front == ID_PLAYER){
        mobAttack(*m);
    }
}

// オブジェクトテーブルをクリア
void clearObjTbl(){
    uchar i, j;

    for(i=0; i<FIELD_SZ; i++){
        for(j=0; j<FIELD_SZ; j++){
            obj_tbl[j][i] = ID_PASSAGE;
        }
    }
}
