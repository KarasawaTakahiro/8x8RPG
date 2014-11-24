/*
    構造体
*/

// プレイヤー
typedef struct player_s{
    uchar dir;      // 進行方向
    uchar x;        // X座標
    uchar y;        // Y座標
    uchar hp;       // 現在HP
    uchar max_hp;   // 最大HP
    uchar attack;   // 攻撃力
    uchar obj_id;   // オブジェクトID
}player_t;

// MOB 敵キャラ
typedef struct mob_s{
    uchar active;       // アクティブ状態
    uchar knockback;    // ノックバック状態
    uchar attack;       // 攻撃力
    uchar x;            // X座標
    uchar y;            // Y座標
    uchar hp;           // 体力
    uchar dir;          // 進行方向
    uchar obj_id;       // オブジェクトID
} mob_t;

// 爆弾
typedef struct bomb_s{
    uchar x;            // X座標
    uchar y;            // Y座標
    uchar timelimit;    // 爆発までのタイムリミット
    uchar attack;       // 攻撃力
    uchar obj_id;       // オブジェクトID
    uchar set;          // 設置済みか
} bomb_t;

