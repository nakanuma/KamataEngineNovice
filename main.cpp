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

Scene scene = STAGESELECT;//初期値を代入。ここを変更でデバッグを行う

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

//プレイヤーが兵士の数字と演算子を選択する関数
void PlayerOpeNumSelect(int x, int y) {
	//加算の場合
	if (x > 520 && x < 568 && y>624 && y < 672 && Novice::IsTriggerMouse(0)) {
		playerOperator = ADDITION;
	}
	//減算の場合
	if (x > 568 && x < 616 && y>624 && y < 672 && Novice::IsTriggerMouse(0)) {
		playerOperator = SUBTRACTION;
	}
	//乗算の場合
	if (x > 616 && x < 664 && y>624 && y < 672 && Novice::IsTriggerMouse(0)) {
		playerOperator = MULTIPLICATION;
	}
	//除算の場合
	if (x > 664 && x < 712 && y>624 && y < 672 && Novice::IsTriggerMouse(0)) {
		playerOperator = DIVISION;
	}

	//1の場合
	if (x > 712 && x < 760 && y>576 && y < 624 && Novice::IsTriggerMouse(0)) {
		playerNumber = ONE;
	}
	//2の場合
	if (x > 712 && x < 760 && y>528 && y < 576 && Novice::IsTriggerMouse(0)) {
		playerNumber = TWO;
	}
	//3の場合
	if (x > 712 && x < 760 && y>480 && y < 528 && Novice::IsTriggerMouse(0)) {
		playerNumber = THREE;
	}
	//4の場合
	if (x > 712 && x < 760 && y>432 && y < 480 && Novice::IsTriggerMouse(0)) {
		playerNumber = FOUR;
	}
}

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
	//int starGH = Novice::LoadTexture("./Resources/images/star.png");//星
	int emptyStarGH = Novice::LoadTexture("./Resources/images/emptyStar.png");//空の星
	int stageSelectFlameGH = Novice::LoadTexture("./Resources/images/stageSelectFlame.png");//ステージを選択する赤枠
	int startButtonGH = Novice::LoadTexture("./Resources/images/startButton.png");//スタートボタン

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
	/// ↓タイトルで使用する変数
	/// 

	//ゲーム開始ボタンと終了ボタンの色を格納
	int gameStartButtonColor,gameEndButtonColor;

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
	int sceneTransitionCount = 45;//シーンが切り替わるまでのカウント。ステージセレクトに戻る際に初期化し直す必要がある

	///
	/// ↓ゲームクリア・オーバー画面で使用する変数
	/// 

	int winLoseAlpha = 0;//勝利と敗北の透明度。初期化の必要あり
	float winLoseY=0.0f;//勝利と敗北のy座標。初期化の必要あり
	int winLoseTimer = 0;//勝利と敗北が動く時間。初期化の必要あり
	bool isWinLoseMove = false;//勝利と敗北が動いているかのフラグ。初期化の必要あり

	int yesButtonColor, noButtonColor;//はいといいえの色を格納

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		//マウスの位置を取得
		Novice::GetMousePosition(&mouseX, &mouseY);

		switch (scene) {

			///
			/// ↓タイトルここから
			/// 
			
		case TITLE:
			///
			/// ↓更新処理ここから
			/// 
			
			//マウスがゲーム開始ボタンの上にある場合の処理
			if (mouseX > 400 && mouseX < 880 && mouseY>432 && mouseY < 528) {
				//押した場合、シーンをステージセレクトに遷移
				if (Novice::IsPressMouse(0)) {
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
			

			//マウスが戻るボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 12 && mouseX < 108 && mouseY>12 && mouseY < 108 && isTransitionBox==false) {
				//押した場合、タイトルに戻る
				if (Novice::IsPressMouse(0)) {
					scene = TITLE;
				}
				//色を濃くする
				returnButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				returnButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ1ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 158 && mouseX < 374 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ1にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE1;
				}
				//色を濃くする
				stage1ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage1ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ2ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 532 && mouseX < 748 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ2にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE2;
				}
				//色を濃くする
				stage2ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage2ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ3ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 906 && mouseX < 1122 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ3にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE3;
				}
				//色を濃くする
				stage3ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage3ButtonColor = 0xFFFFFFFF;
			}

			//マウスがスタートボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 822 && mouseX < 1206 && mouseY>552 && mouseY < 648 && isTransitionBox == false) {
				//押した場合
				if (Novice::IsPressMouse(0)) {
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

			//シーン遷移カウント（45フレーム）が0になったらシーン遷移する
			if (sceneTransitionCount <= 0) {
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

			//スタートボタンを描画
			Novice::DrawSprite(
				822, 552,
				startButtonGH,
				1.0f, 1.0f,
				0.0f,
				startButtonColor
			);

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

				PlayerOpeNumSelect(mouseX, mouseY);//プレイヤーが兵士の演算子と数字を選択する処理

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
						soldier[i].isAlive = false;
						soldier[i].isDead = true;
					}
					//兵士が消滅した際、消滅時のカウントを減らす（画像表示用）
					if (soldier[i].isDead == true) {
						soldier[i].deadCount--;
					}
				}

				//敵の更新処理
				for (int i = 0; i < ENEMY_NUM; i++) {
					//出現時間になった敵が出現
					if (gameCount == enemy[i].summonFlame) {
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
				}

				//兵士と敵が衝突した際の処理
				for (int i = 0; i < SOLDIER_NUM; i++) {
					for (int j = 0; j < ENEMY_NUM; j++) {
						//兵士が敵に衝突しているかつ兵士と敵が生存している場合
						if (soldier[i].pos.x < enemy[j].pos.x + 96 && soldier[i].isAlive == true && enemy[j].isAlive == true) {
							//プレイヤーの演算子が加算のとき
							if (soldier[i].ope == ADDITION) {
								enemy[j].num += soldier[i].num + 1;
								//プレイヤーの演算子が減算のとき
							} else if (soldier[i].ope == SUBTRACTION) {
								enemy[j].num -= soldier[i].num + 1;
								//プレイヤーの演算子が乗算のとき
							} else if (soldier[i].ope == MULTIPLICATION) {
								enemy[j].num *= soldier[i].num + 1;
								//プレイヤーの演算子が除算のとき
							} else if (soldier[i].ope == DIVISION) {
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

				//勝った時の処理
				if (playerTowerHP >= 1 && enemy[15].deadCount <= 0) {
					//プレイヤーの勝利フラグをtrueにする
					isPlayerWin = true;
					//海苔を浮かび上がらせて、alphaが255に到達したらシーンを切り替える
					if (issueAlpha < 255) {
						issueAlpha += 3;
					} else {
						scene = GAMERESULT;
					}
				}

				//負けた時の処理
				if (playerTowerHP <= 0) {
					//海苔を浮かび上がらせて、alphaが255に到達したらシーンを切り替える
					if (issueAlpha < 255) {
						issueAlpha += 3;
					} else {
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

				//浮かび上がる海苔を描画
				Novice::DrawBox(
					0, 0,
					1280, 720,
					0.0f,
					0x00000000 + issueAlpha,
					kFillModeSolid
				);

				//デバッグ用文字列
				Novice::ScreenPrintf(20, 10, "gameCount:%d", gameCount);
				Novice::ScreenPrintf(20, 30, "cooldown:%d", soldierCurrentCooldown);
				Novice::ScreenPrintf(20, 50, "soldierSummonCount:%d", soldierSummonCount);

				///
				/// ↑描画処理ここまで
				///
				
				break;

			case GAMERESULT:

				///
				/// ↓更新処理ここから
				///

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
						//現在のステージによって初期化するステージを変更する
						if (stage == STAGE1) {
							winLoseAlpha = 0;
							winLoseY = 0.0f;
							winLoseTimer = 0;
							isWinLoseMove = false;
							InitializeGameSceneStage1();
							scene = GAME;
						}
						else if (stage == STAGE2) {
							winLoseAlpha = 0;
							winLoseY = 0.0f;
							winLoseTimer = 0;
							isWinLoseMove = false;
							InitializeGameSceneStage2();
							scene = GAME;
						}
						else if (stage == STAGE3) {
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
						if (stage == STAGE1) {
							transitionBoxPosX = 1280;
							isTransitionBox = false;
							sceneTransitionCount = 45;
							winLoseAlpha = 0;
							winLoseY = 0.0f;
							winLoseTimer = 0;
							isWinLoseMove = false;
							InitializeGameSceneStage1();
							scene = STAGESELECT;
						} else if (stage == STAGE2) {
							transitionBoxPosX = 1280;
							isTransitionBox = false;
							sceneTransitionCount = 45;
							winLoseAlpha = 0;
							winLoseY = 0.0f;
							winLoseTimer = 0;
							isWinLoseMove = false;
							InitializeGameSceneStage2();
							scene = STAGESELECT;
						} else if (stage == STAGE3) {
							transitionBoxPosX = 1280;
							isTransitionBox = false;
							sceneTransitionCount = 45;
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
			Novice::DrawBox(
				transitionBoxPosX, 0,
				1920, 720,
				0.0f,
				BLACK,
				kFillModeSolid
			);
			//海苔を移動させる
			if (transitionBoxPosX > -1920) {
				transitionBoxPosX -= 30;
			}
			if (sceneTransitionCount > 0) {
				sceneTransitionCount--;
			}
		}

		//デバッグ用

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
