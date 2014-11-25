/*
    定数群
*/

// 8x8LEDに関する定義
#define LED_SZ 8                // 8x8LED
#define LED_MAX LED_SZ-1        // LEDの最大インデックス値

// フィールドサイズに関する定義
#define FIELD_SZ 16             // ゲームフィールドサイズ
#define FIELD_MAX FIELD_SZ-1    // フィールドサイズの最大インデックス値

// ダンジョンの状態 オブジェクトID
#define ID_PASSAGE  0       // 通路
#define ID_WALL     1       // 壁
#define ID_PLAYER   2       // プレイヤー
#define ID_MOB      3       // 敵
#define ID_BOMB     4       // 爆弾
#define ID_GOAL     5       // ゴール

// 方向の定義
#define DIR_RIGHT   0       // 右
#define DIR_UP      1       // 上
#define DIR_LEFT    2       // 左
#define DIR_DOWN    3       // 下

// プレイヤーの定義
#define PLAYER_MAX_HP   5   // HPの最大値
#define PLAYER_INIT_HP  3   // 初期体力
#define PLAYER_ATTACK   1   // 攻撃力

// マーカーの定義
#define MARKER_SHOW 1       // 表示
#define MARKER_HIDE 0       // 非表示

// 爆弾の定義
#define BOMB_TIMELIMIT  5   // 爆発のタイムリミット[sec]
#define BOMB_ATACK      2   // 攻撃力

// 敵キャラの定義
#define MOB_IDLE        0   // 非アクティブ状態
#define MOB_ACTIVE      1   // アクティブ状態
#define MOB_ATACK       1   // 攻撃力
#define MOB_HP          2   // HP
#define MOB_BORN_NUM    10  // MOBの出現数

// スイッチ
#define SW_1 1              // 値
#define SW_2 2              // 値
#define SW_3 3              // 値

// 雑多定義
#define FALSE       0       // 偽
#define TRUE        1       // 真
#define MOVED       1       // 行動済み
#define UNMOVE      0       // 行動前
#define KNOCKBACK   1       // ノックバックフラグ

#define INCLUDED_CONSTANTS
