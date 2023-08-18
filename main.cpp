#include <Novice.h>
#include "Easing.h"

#define SOLDIER_NUM 10 //召喚する兵士の最大数
#define ENEMY_NUM 16 //出現する敵の最大数

const char kWindowTitle[] = "演算兵士";

//マウスの位置を格納
int mouseX, mouseY;

//ゲームのシーン
enum Scene {
	TITLE,
	STAGESELECT,
	GAME,
	GAMERESULT,
};

Scene scene = TITLE;//初期値を代入。ここを変更でデバッグを行う

//ステージの選択
enum Stage {
	STAGE1,
	STAGE2,
	STAGE3,
};

Stage stage = STAGE1;//初期値を代入

//プレイヤーに持たせる数字と演算子をenumで宣言
enum Number {
	ONE,
	TWO,
	THREE,
	FOUR
};

enum Operator {
	ADDITION,
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION
};

Number playerNumber = ONE; //初期値を代入
Operator playerOperator = ADDITION; //初期値を代入

//位置を表す構造体
struct Vector2 {
	float x;
	float y;
};

//兵士の構造体
struct Soldier {
	Vector2 pos; //位置
	int num; //数字
	int ope; //演算子
	int deadCount; //死亡時の画像表示の秒数
	bool isDead; //死んだかどうか
	bool isAlive; //生存フラグ
};

//兵士の速さ
float soldierSpeed = 2.5f;
//兵士の召喚クールダウン用変数
int soldierCooldownFrames = 64;
int soldierCurrentCooldown = 0;
bool canSummonSoldier = true;

//敵の構造体
struct Enemy {
	Vector2 pos; //位置
	float speed; //速さ
	int num; //数字
	int numTemp; //現在の数字を格納
	int numDigit[3]; //表示用の数字を格納
	int targetNum; //目標の数字（この数字になったら消滅）
	int targetNumTemp; //目標の数字を格納
	int targetNumDigit[3]; //表示用の目標の数字を格納
	int summonFlame; //このフレームになったら出現
	int deadCount; //死亡時の画像表示の秒数
	bool isDead; //死んだかどうか
	bool isAlive; //生存フラグ
};

struct Soldier soldier[SOLDIER_NUM]; //兵士の配列を作成

struct Enemy enemy[ENEMY_NUM]; //敵の配列を作成

int soldierSummonCount; //兵士を何体出したかのカウント

int gameCount; //ゲームの経過フレーム数をカウント

int playerTowerHP; //タワーの体力

float wave1x, wave2x, wave3x; //WAVE数表示画像用のx座標

float waveTextTimer; //WAVE画像を移動させるタイマー

int wave1InTime, wave1OutTime; //WAVE1の画像が入ってくる時間と出ていく時間
int wave2InTime, wave2OutTime; //WAVE2の画像が入ってくる時間と出ていく時間
int wave3InTime, wave3OutTime; //WAVE3の画像が入ってくる時間と出ていく時間

int issueAlpha;//勝敗確定時の浮き出てくる海苔の色

bool isPlayerWin;//プレイヤーが勝利したかのフラグ

//ゲームの初期化処理

//ステージ1用の初期化
void InitializeGameSceneStage1() {
	//兵士の情報を初期化
	for (int i = 0; i < SOLDIER_NUM; i++) {
		soldier[i].pos.x = 1124.0f;
		soldier[i].pos.y = 288.0f;
		soldier[i].num = 0;
		soldier[i].ope = 0;
		soldier[i].deadCount = 64;
		soldier[i].isDead = false;
		soldier[i].isAlive = false;
	}

	//敵の情報初期化用のデータ（STAGE1用）
	float speeds[] = {
		1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, //1~6体目（WAVE1）
		1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, //7~12体目（WAVE2）
		1.0f,1.0f,1.0f,1.0f, //13~16体目（WAVE3）
	};
	int nums[] = {
		1,3,5,4,9,6, //1~6体目（WAVE1）
		5,2,8,9,16,20,//7~12体目（WAVE2）
		2,5,15,30, //13~16体目（WAVE3）
	};
	int targetNums[] = {
		2,5,9,3,6,4, //1~6体目（WAVE1）
		10,8,24,3,8,5,//7~12体目（WAVE2）
		10,12,3,6, //13~16体目（WAVE3）
	};
	int summonFlames[] = {
		320,560,800,1100,1340,1580, //1~6体目（WAVE1）
		2360,2600,2840,3140,3380,3620, //7~12体目（WAVE2）
		4400,4760,5180,5600, //13~16体目（WAVE3）
	};

	//敵の情報を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].pos.x = 60.0f;
		enemy[i].pos.y = 288.0f;
		enemy[i].speed = speeds[i];
		enemy[i].num = nums[i];
		enemy[i].numTemp = 0;
		enemy[i].numDigit[0] = 0;
		enemy[i].numDigit[1] = 0;
		enemy[i].numDigit[2] = 0;
		enemy[i].targetNumTemp = 0;
		enemy[i].targetNumDigit[0] = 0;
		enemy[i].targetNumDigit[1] = 0;
		enemy[i].targetNumDigit[2] = 0;
		enemy[i].targetNum = targetNums[i];
		enemy[i].summonFlame = summonFlames[i];
		enemy[i].deadCount = 64;
		enemy[i].isDead = false;
		enemy[i].isAlive = false;
	}

	//兵士召喚数をカウント
	soldierSummonCount = 0;

	//ゲームの経過フレームをカウント
	gameCount = 0;

	//プレイヤーのタワーのHP
	playerTowerHP = 3;

	//プレイヤーが勝利したかのフラグ
	isPlayerWin = false;

	//WAVE画像を移動させる際のタイマー
	waveTextTimer = 0.0f;

	wave1InTime = 60;//WAVE1の画像が入ってくる時間（ここで変更）
	wave1OutTime = wave1InTime + 120;

	wave2InTime = 2100;//WAVE2の画像が入ってくる時間（ここで変更）
	wave2OutTime = wave2InTime + 120;

	wave3InTime = 4140;//WAVE3の画像が入ってくる時間（ここで変更）
	wave3OutTime = wave3InTime + 120;

	issueAlpha = 0;

	//プレイヤーの演算子と数字を初期化
	playerNumber = ONE;
	playerOperator = ADDITION;
}

//ステージ2用の初期化
void InitializeGameSceneStage2() {
	//兵士の情報を初期化
	for (int i = 0; i < SOLDIER_NUM; i++) {
		soldier[i].pos.x = 1124.0f;
		soldier[i].pos.y = 288.0f;
		soldier[i].num = 0;
		soldier[i].ope = 0;
		soldier[i].deadCount = 64;
		soldier[i].isDead = false;
		soldier[i].isAlive = false;
	}

	//敵の情報初期化用のデータ（STAGE2用）
	float speeds[] = {
		2.0f,2.0f,2.0f,2.0f,1.5f,1.5f, //1~6体目（WAVE1）
		2.0f,2.0f,1.5f,1.5f,1.5f,1.5f, //7~12体目（WAVE2）
		1.5f,1.5f,1.0f,1.0f, //13~16体目（WAVE3）
	};
	int nums[] = {
		6,6,55,27,2,27,//1~6体目（WAVE1）
		720,35,3,6,16,96,//7~12体目（WAVE2）
		32,957,240,128,//13~16体目（WAVE3）
	};
	int targetNums[] = {
		10,4,110,9,32,3,//1~6体目（WAVE1）
		360,140,15,36,2,50,//7~12体目（WAVE2）
		256,320,180,96,//13~16体目（WAVE3）
	};
	int summonFlames[] = {
		320,500,680,860,1100,1460,//1~6体目（WAVE1）
		2160,2400,2700,3000,3300,3600,//7~12体目（WAVE2）
		4380,4740,5280,5760,//13~16体目（WAVE3）
	};


	//敵の情報を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].pos.x = 60.0f;
		enemy[i].pos.y = 288.0f;
		enemy[i].speed = speeds[i];
		enemy[i].num = nums[i];
		enemy[i].numTemp = 0;
		enemy[i].numDigit[0] = 0;
		enemy[i].numDigit[1] = 0;
		enemy[i].numDigit[2] = 0;
		enemy[i].targetNumTemp = 0;
		enemy[i].targetNumDigit[0] = 0;
		enemy[i].targetNumDigit[1] = 0;
		enemy[i].targetNumDigit[2] = 0;
		enemy[i].targetNum = targetNums[i];
		enemy[i].summonFlame = summonFlames[i];
		enemy[i].deadCount = 64;
		enemy[i].isDead = false;
		enemy[i].isAlive = false;
	}

	//兵士召喚数をカウント
	soldierSummonCount = 0;

	//ゲームの経過フレームをカウント
	gameCount = 0; //ここを変更してデバッグ 初期値は0

	//プレイヤーのタワーのHP
	playerTowerHP = 3;

	//プレイヤーが勝利したかのフラグ
	isPlayerWin = false;

	//WAVE画像を移動させる際のタイマー
	waveTextTimer = 0.0f;

	wave1InTime = 60;//WAVE1の画像が入ってくる時間（ここで変更）
	wave1OutTime = wave1InTime + 120;

	wave2InTime = 1900;//WAVE2の画像が入ってくる時間（ここで変更）
	wave2OutTime = wave2InTime + 120;

	wave3InTime = 4120;//WAVE3の画像が入ってくる時間（ここで変更）
	wave3OutTime = wave3InTime + 120;

	issueAlpha = 0;

	//プレイヤーの演算子と数字を初期化
	playerNumber = ONE;
	playerOperator = ADDITION;
}

//ステージ3用の初期化
void InitializeGameSceneStage3() {
	//兵士の情報を初期化
	for (int i = 0; i < SOLDIER_NUM; i++) {
		soldier[i].pos.x = 1124.0f;
		soldier[i].pos.y = 288.0f;
		soldier[i].num = 0;
		soldier[i].ope = 0;
		soldier[i].deadCount = 64;
		soldier[i].isDead = false;
		soldier[i].isAlive = false;
	}

	//敵の情報初期化用のデータ（STAGE3用）
	float speeds[] = {
		1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, //1~6体目（WAVE1）
		1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, //7~12体目（WAVE2）
		1.0f,1.0f,0.5f,0.5f, //13~16体目（WAVE3）
	};
	int nums[] = {
		4,24,3,5,42,90,//1~6体目（WAVE1）
		45,56,600,16,30,28,//7~12体目（WAVE2）
		66,75,25,39,//13~16体目（WAVE3）
	};
	int targetNums[] = {
		32,10,18,36,10,15, //1~6体目（WAVE1）
		18,32,100,60,87,124,//7~12体目（WAVE2）
		88,50,110,60,//13~16体目（WAVE3）
	};
	int summonFlames[] = {
		320,680,1100,1520,1940,2360, //1~6体目（WAVE1）
		3140,3560,3980,4400,4880,5360, //7~12体目（WAVE2）
		6200,6620,7100,7760, //13~16体目（WAVE3）
	};


	//敵の情報を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].pos.x = 60.0f;
		enemy[i].pos.y = 288.0f;
		enemy[i].speed = speeds[i];
		enemy[i].num = nums[i];
		enemy[i].numTemp = 0;
		enemy[i].numDigit[0] = 0;
		enemy[i].numDigit[1] = 0;
		enemy[i].numDigit[2] = 0;
		enemy[i].targetNumTemp = 0;
		enemy[i].targetNumDigit[0] = 0;
		enemy[i].targetNumDigit[1] = 0;
		enemy[i].targetNumDigit[2] = 0;
		enemy[i].targetNum = targetNums[i];
		enemy[i].summonFlame = summonFlames[i];
		enemy[i].deadCount = 64;
		enemy[i].isDead = false;
		enemy[i].isAlive = false;
	}

	//兵士召喚数をカウント
	soldierSummonCount = 0;

	//ゲームの経過フレームをカウント
	gameCount = 0; //ここを変更してデバッグ 初期値は0

	//プレイヤーのタワーのHP
	playerTowerHP = 3;

	//プレイヤーが勝利したかのフラグ
	isPlayerWin = false;

	//WAVE画像を移動させる際のタイマー
	waveTextTimer = 0.0f;

	wave1InTime = 60;//WAVE1の画像が入ってくる時間（ここで変更）
	wave1OutTime = wave1InTime + 120;

	wave2InTime = 2880;//WAVE2の画像が入ってくる時間（ここで変更）
	wave2OutTime = wave2InTime + 120;

	wave3InTime = 5940;//WAVE3の画像が入ってくる時間（ここで変更）
	wave3OutTime = wave3InTime + 120;


	issueAlpha = 0;

	//プレイヤーの演算子と数字を初期化
	playerNumber = ONE;
	playerOperator = ADDITION;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	///
	///画像読み込み
	///

	///
	///↓タイトルで使用する画像
	/// 

	int titleGH = Novice::LoadTexture("./Resources/images/title.png");//タイトル画面
	int gameStartButtonGH = Novice::LoadTexture("./Resources/images/gameStartButton.png");//ゲーム開始ボタン
	int gameEndButtonGH = Novice::LoadTexture("./Resources/images/gameEndButton.png");//終了ボタン

	///
	/// ↓ステージセレクトで使用する画像
	/// 

	int stageSelectGH = Novice::LoadTexture("./Resources/images/stageSelect.png");//ステージ選択画面
	int returnButtonGH = Novice::LoadTexture("./Resources/images/returnButton.png");//戻るボタン
	int stage1ButtonGH = Novice::LoadTexture("./Resources/images/stage1Button.png");//ステージ1のボタン
	int stage2ButtonGH = Novice::LoadTexture("./Resources/images/stage2Button.png");//ステージ2のボタン
	int stage3ButtonGH = Novice::LoadTexture("./Resources/images/stage3Button.png");//ステージ3のボタン
	int starGH = Novice::LoadTexture("./Resources/images/star.png");//星
	int emptyStarGH = Novice::LoadTexture("./Resources/images/emptyStar.png");//空の星
	int stageSelectFlameGH = Novice::LoadTexture("./Resources/images/stageSelectFlame.png");//ステージを選択する赤枠
	int startButtonGH = Novice::LoadTexture("./Resources/images/startButton.png");//スタートボタン
	int conditionsGH = Novice::LoadTexture("./Resources/images/conditions.png");//兵士xx体以下でクリアの画像
	int transitionGH = Novice::LoadTexture("./Resources/images/transition.png");//画面遷移時の背景
	int howToPlayButtonGH = Novice::LoadTexture("./Resources/images/howToPlayButton.png");//遊び方のボタン
	int howToPlayGH = Novice::LoadTexture("./Resources/images/howToPlay.png");//遊び方の説明の画像
	int okButtonGH = Novice::LoadTexture("./Resources/images/okButton.png");//OKボタン

	///
	/// ↓ゲームで使用する画像
	///

	int skyGH = Novice::LoadTexture("./Resources/images/sky.png"); //背景の空
	int groundGH = Novice::LoadTexture("./Resources/images/ground.png"); //背景の地面
	int towerGH = Novice::LoadTexture("./Resources/images/tower.png"); //タワー
	int soldierFlameGH = Novice::LoadTexture("./Resources/images/soldierFlame.png"); //兵士枠（数字、演算子、兵士の状態を表すための枠）
	int selectFlameGH = Novice::LoadTexture("./Resources/images/selectFlame.png"); //数字と演算子の選択中を示す枠
	int soldierGH = Novice::LoadTexture("./Resources/images/soldier.png"); //兵士の画像
	int enemyGH = Novice::LoadTexture("./Resources/images/enemy.png"); //敵の画像
	int soldierCountGH = Novice::LoadTexture("./Resources/images/soldierCount.png"); //兵士出撃数xx体
	int number48xGH[5] = {
		Novice::LoadTexture("./Resources/images/48x0.png"),
		Novice::LoadTexture("./Resources/images/48x1.png"),
		Novice::LoadTexture("./Resources/images/48x2.png"),
		Novice::LoadTexture("./Resources/images/48x3.png"),
		Novice::LoadTexture("./Resources/images/48x4.png"),
	}; //数字（48*48）
	int operator48xGH[4] = {
		Novice::LoadTexture("./Resources/images/48xAddition.png"),
		Novice::LoadTexture("./Resources/images/48xSubtraction.png"),
		Novice::LoadTexture("./Resources/images/48xMultiplication.png"),
		Novice::LoadTexture("./Resources/images/48xDivision.png"),
	}; //演算子（48*48）
	int numberGH[10] = {
		Novice::LoadTexture("./Resources/images/0.png"),
		Novice::LoadTexture("./Resources/images/1.png"),
		Novice::LoadTexture("./Resources/images/2.png"),
		Novice::LoadTexture("./Resources/images/3.png"),
		Novice::LoadTexture("./Resources/images/4.png"),
		Novice::LoadTexture("./Resources/images/5.png"),
		Novice::LoadTexture("./Resources/images/6.png"),
		Novice::LoadTexture("./Resources/images/7.png"),
		Novice::LoadTexture("./Resources/images/8.png"),
		Novice::LoadTexture("./Resources/images/9.png"),
	}; //数字（24*24）
	int operatorGH[4] = {
		Novice::LoadTexture("./Resources/images/addition.png"),
		Novice::LoadTexture("./Resources/images/subtraction.png"),
		Novice::LoadTexture("./Resources/images/multiplication.png"),
		Novice::LoadTexture("./Resources/images/division.png"),
	}; //演算子（24*24）
	int smokeGH = Novice::LoadTexture("./Resources/images/smoke.png"); //死亡時の煙の画像
	int wave1GH = Novice::LoadTexture("./Resources/images/wave1.png"); //WAVE1の画像
	int wave2GH = Novice::LoadTexture("./Resources/images/wave2.png"); //WAVE2の画像
	int wave3GH = Novice::LoadTexture("./Resources/images/wave3.png"); //WAVE3の画像


	///
	/// ↓ゲームクリア・オーバーで使用する画像
	///

	int winGH = Novice::LoadTexture("./Resources/images/win.png");//勝利
	int loseGH = Novice::LoadTexture("./Resources/images/lose.png");//敗北
	int tryAgainGH = Novice::LoadTexture("./Resources/images/tryAgain.png");//もう一度プレイしますか？
	int yesGH = Novice::LoadTexture("./Resources/images/yes.png");//はい
	int noGH = Novice::LoadTexture("./Resources/images/no.png");//いいえ

	///
	/// ↓音読み込み
	/// 

	//BGM
	int titleSH = Novice::LoadAudio("./Resources/sounds/title.wav");//タイトルBGM
	int stageSelectSH = Novice::LoadAudio("./Resources/sounds/stageSelect.wav");//ステージセレクトBGM
	int gameSH = Novice::LoadAudio("./Resources/sounds/game.wav");//ゲームBGM
	int winSH = Novice::LoadAudio("./Resources/sounds/win.wav");//勝利BGM
	int loseSH = Novice::LoadAudio("./Resources/sounds/lose.wav");//敗北BGM

	//効果音
	int button1SH = Novice::LoadAudio("./Resources/sounds/button1.wav");//ボタンを押した時
	int button2SH = Novice::LoadAudio("./Resources/sounds/button2.wav");//ボタンを押した時
	int button3SH = Novice::LoadAudio("./Resources/sounds/button3.wav");//ボタンを押した時
	int button4SH = Novice::LoadAudio("./Resources/sounds/button4.wav");//ボタンを押した時
	int towerDamageSH = Novice::LoadAudio("./Resources/sounds/towerDamage.wav");//タワーに衝突した時
	int damageSH = Novice::LoadAudio("./Resources/sounds/damage.wav");//兵士と敵が消滅した時
	int plusSH = Novice::LoadAudio("./Resources/sounds/plus.wav");//兵士が敵に衝突したとき（加算・乗算）
	int minusSH = Novice::LoadAudio("./Resources/sounds/minus.wav");//兵士が敵に衝突したとき（減算・除算）
	int summonSH = Novice::LoadAudio("./Resources/sounds/summon.wav");//兵士を召喚する時
	int appearingSH = Novice::LoadAudio("./Resources/sounds/appearing.wav");//敵が出現した時
	int waveSH = Novice::LoadAudio("./Resources/sounds/wave.wav");//WAVE表示の音


	int bgmPH = -114514;//音がバグらないようにするための変数

	///
	/// ↓タイトルで使用する変数
	/// 

	//ゲーム開始ボタンと終了ボタンの色を格納
	int gameStartButtonColor, gameEndButtonColor;

	///
	/// ↓ステージセレクトで使用する変数
	/// 

	//戻るボタンの色を格納
	int returnButtonColor;
	//ステージ選択ボタンの色を格納
	int stage1ButtonColor, stage2ButtonColor, stage3ButtonColor;
	//スタートボタンの色を格納
	int startButtonColor;

	//シーン遷移で使用する海苔
	int transitionBoxPosX = 1280;//海苔のx座標。ステージセレクトに戻る際に初期化し直す必要がある
	bool isTransitionBox = false;//シーン遷移が行われているかのフラグ。ステージセレクトに戻る際に初期化し直す必要がある
	int sceneTransitionCount = 60;//シーンが切り替わるまでのカウント。ステージセレクトに戻る際に初期化し直す必要がある

	//遊び方の画像を表示しているかどうかのフラグ
	bool isDisplayHowToPlay = false;
	//遊び方のボタンの色を格納
	int howToPlayButtonColor = 0xFFFFFFFF;
	//OKボタンの色を格納
	int okButtonColor = 0xFFFFFFFF;

	///
	/// ↓ゲームクリア・オーバー画面で使用する変数
	/// 

	int winLoseAlpha = 0;//勝利と敗北の透明度。初期化の必要あり
	float winLoseY = 0.0f;//勝利と敗北のy座標。初期化の必要あり
	int winLoseTimer = 0;//勝利と敗北が動く時間。初期化の必要あり
	bool isWinLoseMove = false;//勝利と敗北が動いているかのフラグ。初期化の必要あり

	int yesButtonColor, noButtonColor;//はいといいえの色を格納

	//取得した星を描画するためのフラグ
	bool stage1Star3 = false;
	bool stage1Star2 = false;
	bool stage1Star1 = false;

	bool stage2Star3 = false;
	bool stage2Star2 = false;
	bool stage2Star1 = false;

	bool stage3Star3 = false;
	bool stage3Star2 = false;
	bool stage3Star1 = false;

	//いいえを押してからスタートを押せるようになるまでのタイマー
	int noTimer = 0;

	//はいといいえを押した際に色を薄くしていく海苔の透明度
	int yesNoAlpha = 0;

	///
	/// ↓ゲームで使用する変数
	/// 

	unsigned int normalColor = 0xFFFFFFFF;      // 通常の色 (白)
	unsigned int translucentColor = 0xFFFFFF40; // 半透明の色

	//ステージ1の星取得条件の色を格納
	int stage1Star3Color = 0xFFFFFFFF;
	int stage1Star2Color = 0xFFFFFFFF;
	int stage1Star1Color = 0xFFFFFFFF;

	//ステージ2の星取得条件の色を格納
	int stage2Star3Color = 0xFFFFFFFF;
	int stage2Star2Color = 0xFFFFFFFF;
	int stage2Star1Color = 0xFFFFFFFF;

	//ステージ3の星取得条件の色を格納
	int stage3Star3Color = 0xFFFFFFFF;
	int stage3Star2Color = 0xFFFFFFFF;
	int stage3Star1Color = 0xFFFFFFFF;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		//マウスの位置を取得
		Novice::GetMousePosition(&mouseX, &mouseY);

		//いいえを押してからスタートを押せるようになるまでのタイマーが0より大きい場合毎フレーム減らす
		if (noTimer > 0) {
			noTimer--;
		}

		//はいといいえを押した際に海苔の色を薄くするための透明度が0より大きい場合、毎フレーム3ずつ減らす
		if (yesNoAlpha > 0) {
			yesNoAlpha -= 5;
		}

		switch (scene) {

			///
			/// ↓タイトルここから
			/// 

		case TITLE:
			///
			/// ↓更新処理ここから
			/// 

			//BGMを流す
			if (bgmPH == -114514) {
				bgmPH = Novice::PlayAudio(titleSH, 1, 1);
			}

			//マウスがゲーム開始ボタンの上にある場合の処理
			if (mouseX > 400 && mouseX < 880 && mouseY>432 && mouseY < 528) {
				//押した場合、シーンをステージセレクトに遷移
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button2SH, false, 1.0f);//ボタンの効果音
					//BGMを止める
					Novice::StopAudio(bgmPH);
					bgmPH = -114514;
					yesNoAlpha = 255;
					scene = STAGESELECT;
				}
				//色を濃くする
				gameStartButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				gameStartButtonColor = 0xFFFFFFFF;
			}

			//マウスが終了ボタンの上にある場合の処理
			if (mouseX > 400 && mouseX < 880 && mouseY>576 && mouseY < 672) {
				//押した場合、ゲームを終了
				if (Novice::IsPressMouse(0)) {
					return 0;
				}
				//色を濃くする
				gameEndButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				gameEndButtonColor = 0xFFFFFFFF;
			}

			///
			/// ↑更新処理ここまで
			/// 

			///
			/// ↓描画処理ここから
			/// 

			//タイトル背景を描画
			Novice::DrawSprite(
				0, 0,
				titleGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//ゲーム開始ボタンを描画
			Novice::DrawSprite(
				400, 432,
				gameStartButtonGH,
				1.0f, 1.0f,
				0.0f,
				gameStartButtonColor
			);

			//ゲーム終了ボタンを描画
			Novice::DrawSprite(
				400, 576,
				gameEndButtonGH,
				1.0f, 1.0f,
				0.0f,
				gameEndButtonColor
			);

			///
			/// ↑描画処理ここまで
			/// 

			break;

			///
			/// ↑タイトルここまで
			/// 

			///
			/// ↓ステージセレクトここから
			/// 

		case STAGESELECT:
			///
			/// ↓更新処理ここから
			/// 

			//BGMを流す
			if (bgmPH == -114514) {
				bgmPH = Novice::PlayAudio(stageSelectSH, 1, 1);
			}

			//マウスが戻るボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			if (mouseX > 12 && mouseX < 108 && mouseY>12 && mouseY < 108 && isTransitionBox == false && isDisplayHowToPlay == false) {
				//押した場合、タイトルに戻る
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button2SH, false, 1.0f);//ボタンの効果音
					//BGMを止める
					Novice::StopAudio(bgmPH);
					bgmPH = -114514;
					yesNoAlpha = 255;
					scene = TITLE;
				}
				//色を濃くする
				returnButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				returnButtonColor = 0xFFFFFFFF;
			}

			//マウスが遊び方ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			if (mouseX > 1022 && mouseX < 1238 && mouseY>24 && mouseY < 96 && isTransitionBox == false && isDisplayHowToPlay == false) {
				//押した場合、遊び方の画像を描画
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button1SH, false, 1.0f);//ボタンの効果音
					isDisplayHowToPlay = true;
				}
				//色を濃くする
				howToPlayButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				howToPlayButtonColor = 0xFFFFFFFF;
			}

			//マウスがOKボタンの上にある場合の処理（isDisplayHowToPlayがtrueのとき）
			if (mouseX > 544 && mouseX < 736 && mouseY>612 && mouseY < 708 && isTransitionBox == false && isDisplayHowToPlay == true) {
				//押した場合、遊び方の画像を描画を終わる
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button1SH, false, 1.0f);//ボタンの効果音
					isDisplayHowToPlay = false;
				}
				//色を濃くする
				okButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				okButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ1ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			if (mouseX > 158 && mouseX < 374 && mouseY>156 && mouseY < 372 && isTransitionBox == false && isDisplayHowToPlay == false) {
				//押した場合、ステージ1にする
				if (Novice::IsPressMouse(0)) {
					//トリガーされていない場合（この条件にしないと長押しでずっと鳴ってしまう）
					if (Novice::IsTriggerMouse(0)) {
						Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
					}
					stage = STAGE1;
				}
				//色を濃くする
				stage1ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage1ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ2ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			if (mouseX > 532 && mouseX < 748 && mouseY>156 && mouseY < 372 && isTransitionBox == false && isDisplayHowToPlay == false) {
				//押した場合、ステージ2にする
				if (Novice::IsPressMouse(0)) {
					//トリガーされていない場合（この条件にしないと長押しでずっと鳴ってしまう）
					if (Novice::IsTriggerMouse(0)) {
						Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
					}
					stage = STAGE2;
				}
				//色を濃くする
				stage2ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage2ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ3ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			if (mouseX > 906 && mouseX < 1122 && mouseY>156 && mouseY < 372 && isTransitionBox == false && isDisplayHowToPlay == false) {
				//押した場合、ステージ3にする
				if (Novice::IsPressMouse(0)) {
					//トリガーされていない場合（この条件にしないと長押しでずっと鳴ってしまう）
					if (Novice::IsTriggerMouse(0)) {
						Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
					}
					stage = STAGE3;
				}
				//色を濃くする
				stage3ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage3ButtonColor = 0xFFFFFFFF;
			}

			//マウスがスタートボタンの上にある場合の処理（シーン遷移時は反応しないようにする）（遊び方を表示している時は反応しないようにする）
			//いいえを押してからスタートを押せるようになるまでのタイマーが0以下の場合という条件を追加
			if (mouseX > 822 && mouseX < 1206 && mouseY>552 && mouseY < 648 && isTransitionBox == false && noTimer <= 0 && isDisplayHowToPlay == false) {
				//押した場合
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button2SH, false, 1.0f);//ボタンの効果音
					//BGMを止める
					Novice::StopAudio(bgmPH);
					//シーン遷移が行われる
					isTransitionBox = true;
					//選択したステージ毎に初期化を行う
					if (stage == STAGE1) {
						InitializeGameSceneStage1();
					}
					if (stage == STAGE2) {
						InitializeGameSceneStage2();
					}
					if (stage == STAGE3) {
						InitializeGameSceneStage3();
					}
				}
				//色を濃くする
				startButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				startButtonColor = 0xFFFFFFFF;
			}

			//シーン遷移カウント（60フレーム）が0になったらシーン遷移する
			if (sceneTransitionCount <= 0) {
				//BGM変数をここで初期化
				bgmPH = -114514;
				scene = GAME;
			}

			///
			/// ↑更新処理ここまで
			/// 

			///
			/// ↓描画処理ここから
			/// 

			//ステージセレクト背景を描画
			Novice::DrawSprite(
				0, 0,
				stageSelectGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//戻るボタンを描画
			Novice::DrawSprite(
				12, 12,
				returnButtonGH,
				1.0f, 1.0f,
				0.0f,
				returnButtonColor
			);

			//遊び方ボタンを描画
			Novice::DrawSprite(
				1022, 24,
				howToPlayButtonGH,
				1.0f, 1.0f,
				0.0f,
				howToPlayButtonColor
			);

			//ステージ1ボタンを描画
			Novice::DrawSprite(
				158, 156,
				stage1ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage1ButtonColor
			);

			//ステージ2ボタンを描画
			Novice::DrawSprite(
				532, 156,
				stage2ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage2ButtonColor
			);

			//ステージ3ボタンを描画
			Novice::DrawSprite(
				906, 156,
				stage3ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage3ButtonColor
			);

			//選択したステージの赤枠を描画（ステージを選択する処理と連動）
			Novice::DrawSprite(
				158 + (stage * 374), 156,
				stageSelectFlameGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//空の星をステージボタンの下に描画
			for (int i = 0; i < 3; i++) {
				//ステージ1の空の星を描画
				Novice::DrawSprite(
					158 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//ステージ2の空の星を描画
				Novice::DrawSprite(
					532 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//ステージ3の空の星を描画
				Novice::DrawSprite(
					906 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//星のフラグがtrueになったら星を描画
			//ステージ1の星
			if (stage1Star3 == true) {
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						158 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage1Star2 == true) {
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						158 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage1Star1 == true) {
				Novice::DrawSprite(
					158, 372,
					starGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//ステージ2の星
			if (stage2Star3 == true) {
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						532 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage2Star2 == true) {
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						532 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage2Star1 == true) {
				Novice::DrawSprite(
					532, 372,
					starGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//ステージ3の星
			if (stage3Star3 == true) {
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						906 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage3Star2 == true) {
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						906 + (i * 72), 372,
						starGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
				}
			}
			if (stage3Star1 == true) {
				Novice::DrawSprite(
					906, 372,
					starGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//スタートボタンを描画
			Novice::DrawSprite(
				822, 552,
				startButtonGH,
				1.0f, 1.0f,
				0.0f,
				startButtonColor
			);

			//星取得条件の描画
			for (int i = 0; i < 3; i++) {
				//兵士xx体以下でクリア
				Novice::DrawSprite(
					119, 513 + (i * 69),
					conditionsGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//上の星を描画
				Novice::DrawSprite(
					515 + (i * 36), 511,
					starGH,
					0.5f, 0.5f,
					0.0f,
					0xFFFFFFFF
				);
			}
			//中の星を描画
			Novice::DrawSprite(
				515, 580,
				starGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			Novice::DrawSprite(
				515 + 36, 580,
				starGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			Novice::DrawSprite(
				515 + 36 + 36, 580,
				emptyStarGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			//下の星を描画
			Novice::DrawSprite(
				515, 649,
				starGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			Novice::DrawSprite(
				515 + 36, 649,
				emptyStarGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			Novice::DrawSprite(
				515 + 36 + 36, 649,
				emptyStarGH,
				0.5f, 0.5f,
				0.0f,
				0xFFFFFFFF
			);
			//ステージ1の場合
			if (stage == STAGE1) {
				//上の段
				//2
				Novice::DrawSprite(
					205, 519,
					numberGH[2],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//0
				Novice::DrawSprite(
					205 + 24, 519,
					numberGH[0],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//中の段
				//2
				Novice::DrawSprite(
					205, 588,
					numberGH[2],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//5
				Novice::DrawSprite(
					205 + 24, 588,
					numberGH[5],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//下の段
				//3
				Novice::DrawSprite(
					205, 657,
					numberGH[3],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//0
				Novice::DrawSprite(
					205 + 24, 657,
					numberGH[0],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}
			//ステージ2の場合
			if (stage == STAGE2) {
				//上の段
				//2
				Novice::DrawSprite(
					205, 519,
					numberGH[2],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//4
				Novice::DrawSprite(
					205 + 24, 519,
					numberGH[4],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//中の段
				//3
				Novice::DrawSprite(
					205, 588,
					numberGH[3],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//0
				Novice::DrawSprite(
					205 + 24, 588,
					numberGH[0],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//下の段
				//3
				Novice::DrawSprite(
					205, 657,
					numberGH[3],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//6
				Novice::DrawSprite(
					205 + 24, 657,
					numberGH[6],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}
			//ステージ3の場合
			if (stage == STAGE3) {
				//上の段
				//3
				Novice::DrawSprite(
					205, 519,
					numberGH[3],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//6
				Novice::DrawSprite(
					205 + 24, 519,
					numberGH[6],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//中の段
				//4
				Novice::DrawSprite(
					205, 588,
					numberGH[4],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//2
				Novice::DrawSprite(
					205 + 24, 588,
					numberGH[2],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//下の段
				//4
				Novice::DrawSprite(
					205, 657,
					numberGH[4],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//8
				Novice::DrawSprite(
					205 + 24, 657,
					numberGH[8],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//遊び方ボタンが押された時、遊び方の画像を描画する
			if (isDisplayHowToPlay == true) {
				Novice::DrawSprite(
					0, 0,
					howToPlayGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//OKボタンを描画する
				Novice::DrawSprite(
					544, 612,
					okButtonGH,
					1.0f, 1.0f,
					0.0f,
					okButtonColor
				);
			}

			///
			/// ↑描画処理ここまで
			/// 

			break;

			///
			/// ↑ステージセレクトここまで
			/// 

			///
			/// ↓ゲームここから
			/// 
		case GAME:
			///
			/// ↓更新処理ここから
			/// 

			//BGMを流す
			if (bgmPH == -114514) {
				bgmPH = Novice::PlayAudio(gameSH, 1, 0.1f);
			}

			//プレイヤーが兵士の数字と演算子を選択する処理
			//加算の場合
			if (mouseX > 520 && mouseX < 568 && mouseY>624 && mouseY < 672 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
				playerOperator = ADDITION;
			}
			//減算の場合
			if (mouseX > 568 && mouseX < 616 && mouseY>624 && mouseY < 672 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
				playerOperator = SUBTRACTION;
			}
			//乗算の場合
			if (mouseX > 616 && mouseX < 664 && mouseY>624 && mouseY < 672 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
				playerOperator = MULTIPLICATION;
			}
			//除算の場合
			if (mouseX > 664 && mouseX < 712 && mouseY>624 && mouseY < 672 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button3SH, false, 1.0f);//ボタンの効果音
				playerOperator = DIVISION;
			}

			//1の場合
			if (mouseX > 712 && mouseX < 760 && mouseY>576 && mouseY < 624 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button4SH, false, 1.0f);//ボタンの効果音
				playerNumber = ONE;
			}
			//2の場合
			if (mouseX > 712 && mouseX < 760 && mouseY>528 && mouseY < 576 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button4SH, false, 1.0f);//ボタンの効果音
				playerNumber = TWO;
			}
			//3の場合
			if (mouseX > 712 && mouseX < 760 && mouseY>480 && mouseY < 528 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button4SH, false, 1.0f);//ボタンの効果音
				playerNumber = THREE;
			}
			//4の場合
			if (mouseX > 712 && mouseX < 760 && mouseY>432 && mouseY < 480 && Novice::IsTriggerMouse(0)) {
				Novice::PlayAudio(button4SH, false, 1.0f);//ボタンの効果音
				playerNumber = FOUR;
			}

			//WAVE1の表示
			if (gameCount > wave1InTime) {
				waveTextTimer++;
			}
			if (waveTextTimer > wave1InTime) {
				waveTextTimer = 60;
			}
			if (gameCount == wave1OutTime) {
				waveTextTimer = 0;
			}

			//WAVE2の表示
			if (gameCount == wave2InTime) {
				waveTextTimer = 0;
			}
			if (gameCount == wave2OutTime) {
				waveTextTimer = 0;
			}

			//WAVE3の表示
			if (gameCount == wave3InTime) {
				waveTextTimer = 0;
			}
			if (gameCount == wave3OutTime) {
				waveTextTimer = 0;
			}

			//プレイヤーが兵士を召喚する処理
			if (mouseX > 520 && mouseX < 712 && mouseY>432 && mouseY < 624 && Novice::IsTriggerMouse(0) && canSummonSoldier) {
				for (int i = 0; i < SOLDIER_NUM; i++) {
					if (soldier[i].isAlive == false) {
						Novice::PlayAudio(summonSH, false, 1.0f);//召喚時の効果音
						soldier[i].isAlive = true;
						soldier[i].isDead = false;
						soldier[i].deadCount = 64;
						soldier[i].pos.x = 1124.0f;
						soldier[i].num = playerNumber;
						soldier[i].ope = playerOperator;
						soldierSummonCount++;
						break;
					}
				}
				//クールダウン（1秒）
				soldierCurrentCooldown = soldierCooldownFrames;
				canSummonSoldier = false;
			}

			//クールダウンのカウントダウン
			if (soldierCurrentCooldown > 0) {
				soldierCurrentCooldown--;
			} else {
				canSummonSoldier = true;
			}

			//兵士の更新処理
			for (int i = 0; i < SOLDIER_NUM; i++) {
				//生きている間移動する処理
				if (soldier[i].isAlive == true) {
					soldier[i].pos.x -= soldierSpeed;
				}
				//兵士が敵のタワーに当たったら消滅する処理
				if (soldier[i].isAlive == true && soldier[i].pos.x < 24 + 168) {
					Novice::PlayAudio(towerDamageSH, false, 1.0f);//タワー衝突時の音
					soldier[i].isAlive = false;
					soldier[i].isDead = true;
				}
				//兵士が消滅した際、消滅時のカウントを減らす（画像表示用）
				if (soldier[i].isDead == true) {
					soldier[i].deadCount--;
				}
				//消滅時に効果音を鳴らす
				if (soldier[i].isDead == true) {
					if (soldier[i].deadCount == 63) {
						Novice::PlayAudio(damageSH, false, 0.5f);//消滅する時の音
					}
				}
			}

			//敵の更新処理
			for (int i = 0; i < ENEMY_NUM; i++) {
				//出現時間になった敵が出現
				if (gameCount == enemy[i].summonFlame) {
					Novice::PlayAudio(appearingSH, false, 1.0f);//敵が出現した時の音
					if (enemy[i].isAlive == false) {
						enemy[i].isAlive = true;
					}
				}
				//生存している敵が移動
				if (enemy[i].isAlive == true) {
					enemy[i].pos.x += enemy[i].speed;
				}
				//敵がプレイヤーのタワーに当たったら消滅する処理
				if (enemy[i].isAlive == true && enemy[i].pos.x > 1088 - 96) {
					Novice::PlayAudio(towerDamageSH, false, 1.0f);//タワー衝突時の音
					playerTowerHP--;
					enemy[i].isAlive = false;
					enemy[i].isDead = true;
				}
				//敵の現在の数字が目標の数字と同じになった場合に消滅する処理
				if (enemy[i].num == enemy[i].targetNum) {
					enemy[i].isAlive = false;
					enemy[i].isDead = true;
				}
				//敵が消滅した際、消滅時のカウントを減らす（画像表示用）
				if (enemy[i].isDead == true) {
					enemy[i].deadCount--;
				}
				//消滅した時の効果音
				if (enemy[i].isDead == true) {
					if (enemy[i].deadCount == 63) {
						Novice::PlayAudio(damageSH, false, 0.5f);//消滅する時の音
					}
				}
			}

			//兵士と敵が衝突した際の処理
			for (int i = 0; i < SOLDIER_NUM; i++) {
				for (int j = 0; j < ENEMY_NUM; j++) {
					//兵士が敵に衝突しているかつ兵士と敵が生存している場合
					if (soldier[i].pos.x < enemy[j].pos.x + 96 && soldier[i].isAlive == true && enemy[j].isAlive == true) {
						//プレイヤーの演算子が加算のとき
						if (soldier[i].ope == ADDITION) {
							Novice::PlayAudio(plusSH, false, 1.0f);//敵に衝突した時
							enemy[j].num += soldier[i].num + 1;
							//プレイヤーの演算子が減算のとき
						} else if (soldier[i].ope == SUBTRACTION) {
							Novice::PlayAudio(minusSH, false, 1.0f);//敵に衝突した時
							enemy[j].num -= soldier[i].num + 1;
							//プレイヤーの演算子が乗算のとき
						} else if (soldier[i].ope == MULTIPLICATION) {
							Novice::PlayAudio(plusSH, false, 1.0f);//敵に衝突した時
							enemy[j].num *= soldier[i].num + 1;
							//プレイヤーの演算子が除算のとき
						} else if (soldier[i].ope == DIVISION) {
							Novice::PlayAudio(minusSH, false, 1.0f);//敵に衝突した時
							enemy[j].num /= soldier[i].num + 1;
						}
						soldier[i].isAlive = false;
						soldier[i].isDead = true;
						break;
					}
				}
			}

			//敵の数字の上限と下限を設定（エラー回避用。兵士と敵の衝突処理の下に置く）
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemy[i].isAlive == true) {
					if (enemy[i].num < 0) {
						enemy[i].num = 0;
					}
					if (enemy[i].num > 999) {
						enemy[i].num = 999;
					}
				}
			}

			//敵の数字を描画するために配列に格納する（衝突した処理の下に置く）
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemy[i].isAlive == true) {
					enemy[i].numTemp = enemy[i].num;
					enemy[i].numDigit[0] = (enemy[i].numTemp % 10);
					enemy[i].numTemp /= 10;
					enemy[i].numDigit[1] = (enemy[i].numTemp % 10);
					enemy[i].numTemp /= 10;
					enemy[i].numDigit[2] = (enemy[i].numTemp % 10);
					enemy[i].numTemp /= 10;
				}
				//敵の目標の数字を描画するために配列に格納する
				if (enemy[i].isAlive == true) {
					enemy[i].targetNumTemp = enemy[i].targetNum;
					enemy[i].targetNumDigit[0] = (enemy[i].targetNumTemp % 10);
					enemy[i].targetNumTemp /= 10;
					enemy[i].targetNumDigit[1] = (enemy[i].targetNumTemp % 10);
					enemy[i].targetNumTemp /= 10;
					enemy[i].targetNumDigit[2] = (enemy[i].targetNumTemp % 10);
					enemy[i].targetNumTemp /= 10;
				}
			}

			//プレイヤーのタワーHPが0未満にならないようにする（エラー回避）
			if (playerTowerHP < 0) {
				playerTowerHP = 0;
			}

			//WAVE表示の際に効果音を鳴らす
			//WAVE1
			if (gameCount == wave1InTime + 50) {
				Novice::PlayAudio(waveSH, false, 1.0f);//wave表示の音
			}
			//WAVE2
			if (gameCount == wave2InTime + 50) {
				Novice::PlayAudio(waveSH, false, 1.0f);//wave表示の音
			}
			//WAVE3
			if (gameCount == wave3InTime + 50) {
				Novice::PlayAudio(waveSH, false, 1.0f);//wave表示の音
			}

			//出現させた兵士の数に応じてテキストの色を透明にする
			//ステージ1の場合
			if (stage == STAGE1) {
				//星3取得条件を20を超えた場合半透明にする
				if (soldierSummonCount > 20) {
					stage1Star3Color = translucentColor;
				} else {
					stage1Star3Color = normalColor;
				}
				//星2取得条件を25を超えた場合半透明にする
				if (soldierSummonCount > 25) {
					stage1Star2Color = translucentColor;
				} else {
					stage1Star2Color = normalColor;
				}
				//星1取得条件を30を超えた場合半透明にする
				if (soldierSummonCount > 30) {
					stage1Star1Color = translucentColor;
				} else {
					stage1Star1Color = normalColor;
				}
			}
			//ステージ2の場合
			if (stage == STAGE2) {
				//星3取得条件を24を超えた場合半透明にする
				if (soldierSummonCount > 24) {
					stage2Star3Color = translucentColor;
				} else {
					stage2Star3Color = normalColor;
				}
				//星2取得条件を30を超えた場合半透明にする
				if (soldierSummonCount > 30) {
					stage2Star2Color = translucentColor;
				} else {
					stage2Star2Color = normalColor;
				}
				//星1取得条件を36を超えた場合半透明にする
				if (soldierSummonCount > 36) {
					stage2Star1Color = translucentColor;
				} else {
					stage2Star1Color = normalColor;
				}
			}
			//ステージ3の場合
			if (stage == STAGE3) {
				//星3取得条件を24を超えた場合半透明にする
				if (soldierSummonCount > 36) {
					stage3Star3Color = translucentColor;
				} else {
					stage3Star3Color = normalColor;
				}
				//星2取得条件を30を超えた場合半透明にする
				if (soldierSummonCount > 42) {
					stage3Star2Color = translucentColor;
				} else {
					stage3Star2Color = normalColor;
				}
				//星1取得条件を36を超えた場合半透明にする
				if (soldierSummonCount > 48) {
					stage3Star1Color = translucentColor;
				} else {
					stage3Star1Color = normalColor;
				}
			}

			//勝った時の処理
			if (playerTowerHP >= 1 && enemy[15].deadCount <= 0) {
				//プレイヤーの勝利フラグをtrueにする
				isPlayerWin = true;
				//海苔を浮かび上がらせて、alphaが255に到達したらシーンを切り替える
				if (issueAlpha < 255) {
					issueAlpha += 3;
				} else {
					//音楽を止める
					Novice::StopAudio(bgmPH);
					bgmPH = -114514;
					scene = GAMERESULT;
				}
			}

			//負けた時の処理
			if (playerTowerHP <= 0) {
				//海苔を浮かび上がらせて、alphaが255に到達したらシーンを切り替える
				if (issueAlpha < 255) {
					issueAlpha += 3;
				} else {
					//音楽を止める
					Novice::StopAudio(bgmPH);
					bgmPH = -114514;
					scene = GAMERESULT;
				}
			}

			gameCount++; //1ループ毎にゲームの経過フレームを増加

			///
			/// ↑更新処理ここまで
			/// 

			///
			/// ↓描画処理ここから
			///

			//背景の空を描画
			Novice::DrawSprite(
				0, 0,
				skyGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//背景の地面を描画
			Novice::DrawSprite(
				0, 384,
				groundGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//敵のタワーを描画
			Novice::DrawSprite(
				24, 72,
				towerGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//プレイヤーのタワーを描画
			Novice::DrawSprite(
				1088, 72,
				towerGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//プレイヤーのタワーのHPを描画
			Novice::DrawSprite(
				1136, 36,
				numberGH[playerTowerHP],
				1.0f, 1.0f,
				0.0f,
				0x000000FF
			);//残りHP
			Novice::DrawSprite(
				1136 + 24, 36,
				operatorGH[3],
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);//真ん中の記号
			Novice::DrawSprite(
				1136 + 24 + 24, 36,
				numberGH[3],
				1.0f, 1.0f,
				0.0f,
				0x000000FF
			);//HPの最大数（3）

			//兵士枠を描画
			Novice::DrawSprite(
				520, 432,
				soldierFlameGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//選択した数字の赤枠を描画（プレイヤーが数字を選択する処理と連動）
			Novice::DrawSprite(
				712 - 1, 576 + 1 - (playerNumber * 48),
				selectFlameGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//選択した演算子の赤枠を描画（プレイヤーが演算子を選択する処理と連動）
			Novice::DrawSprite(
				520 + 1 + (playerOperator * 48), 624 - 1,
				selectFlameGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//兵士枠（真ん中の枠）に数字を描画（プレイヤーが数字を選択する処理と連動）
			Novice::DrawSprite(
				616, 504,
				number48xGH[playerNumber + 1],
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//兵士枠（真ん中の枠）に演算子を描画（プレイヤーが演算子を選択する処理と連動）
			Novice::DrawSprite(
				568, 504,
				operator48xGH[playerOperator],
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//召喚された兵士を描画
			for (int i = 0; i < SOLDIER_NUM; i++) {
				if (soldier[i].isAlive == true) {
					//箱を描画
					Novice::DrawSprite(
						(int)soldier[i].pos.x, (int)soldier[i].pos.y,
						soldierGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
					//演算子を描画
					Novice::DrawSprite(
						(int)soldier[i].pos.x + 24, (int)soldier[i].pos.y + 36,
						operatorGH[soldier[i].ope],
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
					//数字を描画
					Novice::DrawSprite(
						(int)soldier[i].pos.x + 24 + 24, (int)soldier[i].pos.y + 36,
						numberGH[soldier[i].num + 1],
						1.0f, 1.0f,
						0.0f,
						0x000000FF
					);
					//死亡した際に煙のアニメーションを描画
				} else if (soldier[i].isDead == true && soldier[i].deadCount > 0) {
					for (int j = 0; j < 8; j++) {
						if (soldier[i].deadCount <= 64 - (j * 8) && soldier[i].deadCount >= 64 - ((j + 1) * 8)) {
							Novice::DrawSpriteRect(
								(int)soldier[i].pos.x, (int)soldier[i].pos.y,
								0 + (96 * j), 0,
								96, 96,
								smokeGH,
								0.125f, 1.0f,
								0.0f,
								0xFFFFFFFF
							);
						}
					}
				}

			}

			//兵士召喚のクールタイム表示
			for (int i = 0; i < soldierCurrentCooldown; i++) {
				Novice::DrawBox(
					520 + (i * 3), 432,
					3, 192,
					0.0f,
					0x000000C0,
					kFillModeSolid
				);
			}

			//出現した敵を描画
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemy[i].isAlive == true) {
					Novice::DrawSprite(
						(int)enemy[i].pos.x, (int)enemy[i].pos.y,
						enemyGH,
						1.0f, 1.0f,
						0.0f,
						0xFFFFFFFF
					);
					//数字を描画
					for (int j = 0; j < 3; j++) {
						Novice::DrawSprite(
							(int)enemy[i].pos.x + 12 + (j * 24), (int)enemy[i].pos.y + 12,
							numberGH[enemy[i].numDigit[2 - j]],
							1.0f, 1.0f,
							0.0f,
							0x000000FF
						);
						//目標の数字を描画
						Novice::DrawSprite(
							(int)enemy[i].pos.x + 12 + (j * 24), (int)enemy[i].pos.y + 60,
							numberGH[enemy[i].targetNumDigit[2 - j]],
							1.0f, 1.0f,
							0.0f,
							0xFF0000FF
						);
					}
					//死亡した際に煙のアニメーションを描画
				} else if (enemy[i].isDead == true && enemy[i].deadCount > 0) {
					for (int j = 0; j < 8; j++) {
						if (enemy[i].deadCount <= 64 - (j * 8) && enemy[i].deadCount >= 64 - ((j + 1) * 8)) {
							Novice::DrawSpriteRect(
								(int)enemy[i].pos.x, (int)enemy[i].pos.y,
								0 + (96 * j), 0,
								96, 96,
								smokeGH,
								0.125f, 1.0f,
								0.0f,
								0xFFFFFFFF
							);
						}
					}
				}
			}

			//WAVE1のx座標を移動
			wave1x = EaseInQuartPos(1280.0f, 472.0f, waveTextTimer / 60.0f);
			if (gameCount > wave1OutTime) {
				wave1x = EaseInQuartPos(472.0f, -472.0f, waveTextTimer / 60.0f);
			}

			if (gameCount < wave2InTime - 200) {
				Novice::DrawSprite(
					(int)wave1x, 168,
					wave1GH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//WAVE2のx座標を移動
			wave2x = EaseInQuartPos(1280.0f, 472.0f, waveTextTimer / 60.0f);
			if (gameCount > wave2OutTime) {
				wave2x = EaseInQuartPos(472.0f, -472.0f, waveTextTimer / 60.0f);
			}

			if (gameCount > wave2InTime && gameCount < wave3InTime - 200) {
				Novice::DrawSprite(
					(int)wave2x, 168,
					wave2GH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//WAVE3のx座標を移動
			wave3x = EaseInQuartPos(1280.0f, 472.0f, waveTextTimer / 60.0f);
			if (gameCount > wave3OutTime) {
				wave3x = EaseInQuartPos(472.0f, -472.0f, waveTextTimer / 60.0f);
			}

			if (gameCount > wave3InTime) {
				Novice::DrawSprite(
					(int)wave3x, 168,
					wave3GH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//兵士の出撃数を描画
			Novice::DrawSprite(
				930, 426,
				soldierCountGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);
			//兵士に使っている画像を描画
			Novice::DrawSprite(
				972, 504 - 20,
				soldierGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);
			//10の位を描画
			Novice::DrawSprite(
				972 + 24, 504 + 36 - 20,
				numberGH[soldierSummonCount / 10 % 10],
				1.0f, 1.0f,
				0.0f,
				0x000000FF
			);
			//1の位を描画
			Novice::DrawSprite(
				972 + 24 + 24, 504 + 36 - 20,
				numberGH[soldierSummonCount % 10],
				1.0f, 1.0f,
				0.0f,
				0x000000FF
			);

			//星取得条件を描画
			//ステージ1の場合
			if (stage == STAGE1) {
				//星3の条件
				Novice::DrawSprite(
					58, 461,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage1Star3Color
				);
				//2
				Novice::DrawSprite(
					123, 467,
					numberGH[2],
					0.75f, 0.75f,
					0.0f,
					stage1Star3Color
				);
				//0
				Novice::DrawSprite(
					123 + 18, 467,
					numberGH[0],
					0.75f, 0.75f,
					0.0f,
					stage1Star3Color
				);
				//星を描画
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 455,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage1Star3Color
					);
				}

				//星2の条件
				Novice::DrawSprite(
					58, 538,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage1Star2Color
				);
				//2
				Novice::DrawSprite(
					123, 544,
					numberGH[2],
					0.75f, 0.75f,
					0.0f,
					stage1Star2Color
				);
				//5
				Novice::DrawSprite(
					123 + 18, 544,
					numberGH[5],
					0.75f, 0.75f,
					0.0f,
					stage1Star2Color
				);
				//星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 532,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage1Star2Color
					);
				}
				//空の星を描画
				Novice::DrawSprite(
					427, 532,
					emptyStarGH,
					0.5f, 0.5f,
					0.0f,
					stage1Star2Color
				);

				//星1の条件
				Novice::DrawSprite(
					58, 615,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage1Star1Color
				);
				//3
				Novice::DrawSprite(
					123, 621,
					numberGH[3],
					0.75f, 0.75f,
					0.0f,
					stage1Star1Color
				);
				//0
				Novice::DrawSprite(
					123 + 18, 621,
					numberGH[0],
					0.75f, 0.75f,
					0.0f,
					stage1Star1Color
				);
				//星を描画
				Novice::DrawSprite(
					355, 609,
					starGH,
					0.5f, 0.5f,
					0.0f,
					stage1Star1Color
				);
				//空の星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						391 + (i * 36), 609,
						emptyStarGH,
						0.5f, 0.5f,
						0.0f,
						stage1Star1Color
					);
				}
			}

			//ステージ2の場合
			if (stage == STAGE2) {
				//星3の条件
				Novice::DrawSprite(
					58, 461,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage2Star3Color
				);
				//2
				Novice::DrawSprite(
					123, 467,
					numberGH[2],
					0.75f, 0.75f,
					0.0f,
					stage2Star3Color
				);
				//4
				Novice::DrawSprite(
					123 + 18, 467,
					numberGH[4],
					0.75f, 0.75f,
					0.0f,
					stage2Star3Color
				);
				//星を描画
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 455,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage2Star3Color
					);
				}

				//星2の条件
				Novice::DrawSprite(
					58, 538,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage2Star2Color
				);
				//3
				Novice::DrawSprite(
					123, 544,
					numberGH[3],
					0.75f, 0.75f,
					0.0f,
					stage2Star2Color
				);
				//0
				Novice::DrawSprite(
					123 + 18, 544,
					numberGH[0],
					0.75f, 0.75f,
					0.0f,
					stage2Star2Color
				);
				//星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 532,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage2Star2Color
					);
				}
				//空の星を描画
				Novice::DrawSprite(
					427, 532,
					emptyStarGH,
					0.5f, 0.5f,
					0.0f,
					stage2Star2Color
				);

				//星1の条件
				Novice::DrawSprite(
					58, 615,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage2Star1Color
				);
				//3
				Novice::DrawSprite(
					123, 621,
					numberGH[3],
					0.75f, 0.75f,
					0.0f,
					stage2Star1Color
				);
				//6
				Novice::DrawSprite(
					123 + 18, 621,
					numberGH[6],
					0.75f, 0.75f,
					0.0f,
					stage2Star1Color
				);
				//星を描画
				Novice::DrawSprite(
					355, 609,
					starGH,
					0.5f, 0.5f,
					0.0f,
					stage2Star1Color
				);
				//空の星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						391 + (i * 36), 609,
						emptyStarGH,
						0.5f, 0.5f,
						0.0f,
						stage2Star1Color
					);
				}
			}

			//ステージ3の場合
			if (stage == STAGE3) {
				//星3の条件
				Novice::DrawSprite(
					58, 461,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage3Star3Color
				);
				//3
				Novice::DrawSprite(
					123, 467,
					numberGH[3],
					0.75f, 0.75f,
					0.0f,
					stage3Star3Color
				);
				//6
				Novice::DrawSprite(
					123 + 18, 467,
					numberGH[6],
					0.75f, 0.75f,
					0.0f,
					stage3Star3Color
				);
				//星を描画
				for (int i = 0; i < 3; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 455,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage3Star3Color
					);
				}

				//星2の条件
				Novice::DrawSprite(
					58, 538,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage3Star2Color
				);
				//4
				Novice::DrawSprite(
					123, 544,
					numberGH[4],
					0.75f, 0.75f,
					0.0f,
					stage3Star2Color
				);
				//2
				Novice::DrawSprite(
					123 + 18, 544,
					numberGH[2],
					0.75f, 0.75f,
					0.0f,
					stage3Star2Color
				);
				//星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						355 + (i * 36), 532,
						starGH,
						0.5f, 0.5f,
						0.0f,
						stage3Star2Color
					);
				}
				//空の星を描画
				Novice::DrawSprite(
					427, 532,
					emptyStarGH,
					0.5f, 0.5f,
					0.0f,
					stage3Star2Color
				);

				//星1の条件
				Novice::DrawSprite(
					58, 615,
					conditionsGH,
					0.75f, 0.75f,
					0.0f,
					stage3Star1Color
				);
				//4
				Novice::DrawSprite(
					123, 621,
					numberGH[4],
					0.75f, 0.75f,
					0.0f,
					stage3Star1Color
				);
				//8
				Novice::DrawSprite(
					123 + 18, 621,
					numberGH[8],
					0.75f, 0.75f,
					0.0f,
					stage3Star1Color
				);
				//星を描画
				Novice::DrawSprite(
					355, 609,
					starGH,
					0.5f, 0.5f,
					0.0f,
					stage3Star1Color
				);
				//空の星を描画
				for (int i = 0; i < 2; i++) {
					Novice::DrawSprite(
						391 + (i * 36), 609,
						emptyStarGH,
						0.5f, 0.5f,
						0.0f,
						stage3Star1Color
					);
				}
			}

			//浮かび上がる海苔を描画
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0x00000000 + issueAlpha,
				kFillModeSolid
			);

			//デバッグ用文字列
			/*Novice::ScreenPrintf(20, 10, "gameCount:%d", gameCount);
			Novice::ScreenPrintf(20, 30, "cooldown:%d", soldierCurrentCooldown);
			Novice::ScreenPrintf(20, 50, "soldierSummonCount:%d", soldierSummonCount);*/

			///
			/// ↑描画処理ここまで
			///

			break;

		case GAMERESULT:

			///
			/// ↓更新処理ここから
			///

			//BGMを流す
			if (bgmPH == -114514) {
				//勝利の場合
				if (isPlayerWin == true) {
					bgmPH = Novice::PlayAudio(winSH, 0, 1);
					//敗北の場合
				} else {
					bgmPH = Novice::PlayAudio(loseSH, 0, 1);
				}
			}

			//実績を解除して星フラグを操作
			//ステージ1の場合
			if (stage == STAGE1) {
				if (isPlayerWin == true && soldierSummonCount <= 20) {
					stage1Star3 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 21 && soldierSummonCount <= 25) {
					stage1Star2 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 26 && soldierSummonCount <= 30) {
					stage1Star1 = true;
				}
			}
			//ステージ2の場合
			if (stage == STAGE2) {
				if (isPlayerWin == true && soldierSummonCount <= 24) {
					stage2Star3 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 25 && soldierSummonCount <= 30) {
					stage2Star2 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 31 && soldierSummonCount <= 36) {
					stage2Star1 = true;
				}
			}
			//ステージ3の場合
			if (stage == STAGE3) {
				if (isPlayerWin == true && soldierSummonCount <= 36) {
					stage3Star3 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 37 && soldierSummonCount <= 42) {
					stage3Star2 = true;
				}
				if (isPlayerWin == true && soldierSummonCount >= 43 && soldierSummonCount <= 48) {
					stage3Star1 = true;
				}
			}

			//透明度が255になるまで増加
			if (winLoseAlpha < 255) {
				winLoseAlpha += 5;
				//透明度が255に到達したら画像が動き始める
			} else {
				isWinLoseMove = true;
			}

			//透明度が255に到達したら画像が動き始める
			if (isWinLoseMove == true) {
				if (winLoseTimer < 60) {
					winLoseTimer++;
				}
			}

			//勝敗の文字のY座標を更新
			winLoseY = EaseInQuartPos(312, 132, winLoseTimer / 60.0f);

			//マウスがはいボタンの上にある場合の処理
			if (mouseX > 288 && mouseX < 576 && mouseY>492 && mouseY < 598) {
				//押した場合、ゲームを初期化してゲームシーンに戻る
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button2SH, false, 1.0f);//ボタンの効果音
					//BGMの変数を初期化
					bgmPH = -114514;
					//はいといいえを押した際に海苔の色を薄くするための透明度を255で初期化
					yesNoAlpha = 255;
					//現在のステージによって初期化するステージを変更する
					if (stage == STAGE1) {
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage1();
						scene = GAME;
					} else if (stage == STAGE2) {
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage2();
						scene = GAME;
					} else if (stage == STAGE3) {
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage3();
						scene = GAME;
					}
				}
				//色を濃くする
				yesButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				yesButtonColor = 0xFFFFFFFF;
			}

			//マウスがいいえボタンの上にある場合の処理
			if (mouseX > 704 && mouseX < 992 && mouseY>492 && mouseY < 598) {
				//押した場合、ステージセレクトに戻る（初期化する）
				if (Novice::IsPressMouse(0)) {
					Novice::PlayAudio(button2SH, false, 1.0f);//ボタンの効果音
					//BGMの変数を初期化
					bgmPH = -114514;
					//いいえを押してからスタートを押せるようになるまでのタイマーを30で初期化
					noTimer = 30;
					//はいといいえを押した際に海苔の色を薄くするための透明度を255で初期化
					yesNoAlpha = 255;
					if (stage == STAGE1) {
						transitionBoxPosX = 1280;
						isTransitionBox = false;
						sceneTransitionCount = 60;
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage1();
						scene = STAGESELECT;
					} else if (stage == STAGE2) {
						transitionBoxPosX = 1280;
						isTransitionBox = false;
						sceneTransitionCount = 60;
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage2();
						scene = STAGESELECT;
					} else if (stage == STAGE3) {
						transitionBoxPosX = 1280;
						isTransitionBox = false;
						sceneTransitionCount = 60;
						winLoseAlpha = 0;
						winLoseY = 0.0f;
						winLoseTimer = 0;
						isWinLoseMove = false;
						InitializeGameSceneStage3();
						scene = STAGESELECT;
					}
				}
				//色を濃くする
				noButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				noButtonColor = 0xFFFFFFFF;
			}

			///
			/// ↑更新処理ここまで
			///

			///
			/// ↓描画処理ここから
			///

			//黒背景を描画
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				BLACK,
				kFillModeSolid
			);

			//勝っていれば勝利、負けていれば敗北を描画
			if (playerTowerHP >= 1 && enemy[15].deadCount <= 0) {
				Novice::DrawSprite(
					496, (int)winLoseY,
					winGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFF00 + winLoseAlpha
				);
			} else {
				Novice::DrawSprite(
					496, (int)winLoseY,
					loseGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFF00 + winLoseAlpha
				);
			}

			//勝利と敗北の画像が移動しきった場合の処理
			if (winLoseTimer >= 60) {
				//もう一度プレイしますか？を描画
				Novice::DrawSprite(
					352, 336,
					tryAgainGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//はいを描画
				Novice::DrawSprite(
					288, 492,
					yesGH,
					1.0f, 1.0f,
					0.0f,
					yesButtonColor
				);
				//いいえを描画
				Novice::DrawSprite(
					704, 492,
					noGH,
					1.0f, 1.0f,
					0.0f,
					noButtonColor
				);
			}

			///
			/// ↑描画処理ここまで
			///

			break;
		}

		//ステージセレクト画面のシーン遷移で使用する海苔を描画
		if (isTransitionBox == true) {
			Novice::DrawSprite(
				transitionBoxPosX, 0,
				transitionGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);
			//海苔を移動させる
			if (transitionBoxPosX > -2160) {
				transitionBoxPosX -= 30;
			}
			if (sceneTransitionCount > 0) {
				sceneTransitionCount--;
			}
		}

		//はいといいえを押した際に表示する海苔を描画
		Novice::DrawBox(
			0, 0,
			1280, 720,
			0.0f,
			0x000000 + yesNoAlpha,
			kFillModeSolid
		);

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
