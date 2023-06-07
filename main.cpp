#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PLAYER_BULLET_NUM 20   //プレイヤーが発射する弾の最大数
#define ENEMY_NUM 100          //敵の最大数
#define ENEMY_BULLET_NUM 1000  //敵が発射する弾の最大数

const char kWindowTitle[] = "shooting";

//プレイヤーの情報
float playerPosX = 330.0f;         //X座標
float playerPosY = 640.0f;         //Y座標
float playerR = 16.0f;             //半径
float playerSpd = 6.0f;            //速度
int playerScore = 0;               //現在のスコア
int playerHP = 3;                  //プレイヤーの体力
bool isPlayerInvincible = false;   //プレイヤーが無敵状態かのフラグ
int playerInvincibleCount = 0;     //プレイヤーの無敵時間カウント

//プレイヤーのキー移動
float velX = 0;
float velY = 0;

//プレイヤーの弾の情報
float playerBulletPosX[PLAYER_BULLET_NUM];          //X座標
float playerBulletPosY[PLAYER_BULLET_NUM];          //Y座標
bool isPlayerBulletShot[PLAYER_BULLET_NUM];         //プレイヤーの弾が撃たれているかのフラグ
float playerBulletR = 8.0f;                         //半径
float playerBulletSpd = 32.0f;                      //速度

//敵の情報
float enemyPosX[ENEMY_NUM];      //X座標
float enemyPosY[ENEMY_NUM];      //Y座標
bool isEnemyAlive[ENEMY_NUM];    //敵が生存しているかのフラグ
float enemyR = 16.0f;            //半径
float enemySpd[ENEMY_NUM];       //速度
float enemyReturnSpd[ENEMY_NUM]; //帰還する時の速度
int enemyDeadCount[ENEMY_NUM];   //敵消滅時のカウント
int enemyHP[ENEMY_NUM];          //敵の体力
int enemyTimeCount[ENEMY_NUM];   //敵が出現した後のカウント
int enemyRapidCount = 60;        //敵の射撃間隔のカウント
int enemyReturnCount[ENEMY_NUM]; //帰還カウント

//敵の弾の情報
float enemyBulletPosX[ENEMY_BULLET_NUM];  //X座標
float enemyBulletPosY[ENEMY_BULLET_NUM];  //Y座標
bool isEnemyBulletShot[ENEMY_BULLET_NUM]; //弾が撃たれているかのフラグ
float enemyBulletR = 12.0f;               //半径
float enemyBulletSpd = 5.0f;              //速度
float enemyBulletVecX[ENEMY_BULLET_NUM];  //X方向のベクトル
float enemyBulletVecY[ENEMY_BULLET_NUM];  //Y方向のベクトル
int enemyBulletGHindex[ENEMY_BULLET_NUM];

//ボスの情報
float bossPosX = 330.f;  //X座標
float bossPosY = -64.0;  //Y座標
float bossR = 32.f;      //半径
int bossHP = 1000;       //体力
bool isBossAlive;        //生存しているかのフラグ

//ゲームの残り時間
int gameLeftTime = 60;
long long gameCount = 0;

//ずっとカウントする
long long allTheTimeCount = 0;

bool isSpiralActive;
float spiralAngle = 0;

void InitializeGameScene() {
	//プレイヤーの情報
	playerPosX = 330.0f;         //X座標
	playerPosY = 640.0f;         //Y座標
	playerR = 16.0f;             //半径
	playerSpd = 6.0f;            //速度
	playerScore = 0;               //現在のスコア
	playerHP = 3;                  //プレイヤーの体力
	isPlayerInvincible = false;   //プレイヤーが無敵状態かのフラグ
	playerInvincibleCount = 0;     //プレイヤーの無敵時間カウント

	//プレイヤーのキー移動
	velX = 0;
	velY = 0;

	//プレイヤーの弾の情報
	playerBulletR = 8.0f;                                //半径
	playerBulletSpd = 32.0f;                             //速度

	//プレイヤーの弾の情報の配列を初期化
	for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
		playerBulletPosX[i] = 0.0f;
		playerBulletPosY[i] = 0.0f;
		isPlayerBulletShot[i] = false;
	}

	//敵の情報
	enemyR = 16.0f;            //半径
	enemyRapidCount = 40;        //敵の射撃間隔のカウント

	//敵の座標を決める為の乱数の最小値と最大値
	int lower = 20;
	int upper = 640;

	//敵の情報の配列を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemyDeadCount[i] = 30;
		//敵の体力を設定
		enemyHP[i] = 2;
		//敵のX座標を決めるための乱数（20~640）を生成
		int random_number = (rand() % (upper - lower + 1)) + lower;
		enemyPosX[i] = (float)random_number;
		//Y座標を80ずつ上にずらして生成
		enemyPosY[i] = -80 + (-80.0f * i);
		isEnemyAlive[i] = true;
		enemySpd[i] = 2.0f;
		enemyReturnSpd[i] = 4.0f;
		enemyReturnCount[i] = 120;
		enemyTimeCount[i] = 0;
	}

	//敵の弾の情報を初期化
	for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
		enemyBulletPosX[i] = 0.0f;
		enemyBulletPosY[i] = 0.0f;
		isEnemyBulletShot[i] = false;
		enemyBulletVecX[i] = 0.0f;
		enemyBulletVecY[i] = 0.0f;
		enemyBulletGHindex[i] = 0;
	}

	//ボスの情報
	bossPosX = 330.f;  //X座標
	bossPosY = -64.0;  //Y座標
	bossR = 32.f;      //半径
	bossHP = 1000;     //体力
	isBossAlive = true;//生存フラグ

	isSpiralActive = false;
	spiralAngle = 0;

	//ゲームの残り時間
	gameLeftTime = 60; /*ここで時間によるデバッグを行う*/
	gameCount = 0;

	return;
}

void EnemyShot(float posX, float posY) {
	for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
		//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
		if (!isEnemyBulletShot[j]) {
			//プレイヤーに向かって移動するベクトルを計算
			float xv = playerPosX - posX;
			float yv = playerPosY - posY;
			float v = sqrtf(xv * xv + yv * yv);
			enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
			enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
			enemyBulletPosX[j] = posX;
			enemyBulletPosY[j] = posY;
			isEnemyBulletShot[j] = true;
			enemyBulletGHindex[j] = 0;
			break;
		}
	}
}

void Enemy3Way(float posX, float posY) {
	//真ん中の一発を撃つ
	for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
		//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
		if (!isEnemyBulletShot[j]) {
			//プレイヤーに向かって移動するベクトルを計算
			float xv = playerPosX - posX;
			float yv = playerPosY - posY;
			float v = sqrtf(xv * xv + yv * yv);
			enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
			enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
			enemyBulletPosX[j] = posX;
			enemyBulletPosY[j] = posY;
			isEnemyBulletShot[j] = true;
			enemyBulletGHindex[j] = 1;
			break;
		}
	}

	//弾の間の角度
	float angle = 30.f * (float)M_PI / 180.f;

	//左の弾を撃つ
	for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
		//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
		if (!isEnemyBulletShot[j]) {
			//プレイヤーに向かって移動するベクトルを計算

			float vx, vy;
			vx = playerPosX - posX;
			vy = playerPosY - posY;
			float xv = vx * cosf(angle) - vy * sinf(angle);
			float yv = vy * cosf(angle) + vx * sinf(angle);
			float v = sqrtf(xv * xv + yv * yv);
			enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
			enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
			enemyBulletPosX[j] = posX;
			enemyBulletPosY[j] = posY;
			isEnemyBulletShot[j] = true;
			enemyBulletGHindex[j] = 1;
			break;
		}
	}

	//右の弾を撃つ
	for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
		//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
		if (!isEnemyBulletShot[j]) {
			//プレイヤーに向かって移動するベクトルを計算

			float vx, vy;
			vx = playerPosX - posX;
			vy = playerPosY - posY;
			float xv = vx * cosf(-angle) - vy * sinf(-angle);
			float yv = vy * cosf(-angle) + vx * sinf(-angle);
			float v = sqrtf(xv * xv + yv * yv);
			enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
			enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
			enemyBulletPosX[j] = posX;
			enemyBulletPosY[j] = posY;
			isEnemyBulletShot[j] = true;
			enemyBulletGHindex[j] = 1;
			break;
		}
	}
}

void EnemyRandom(float posX, float posY) {
	for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
		//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
		if (!isEnemyBulletShot[j]) {
			//プレイヤーに向かって移動するベクトルを計算
			float xv = float(rand() - RAND_MAX / 2);
			float yv = float(rand() - RAND_MAX / 2);
			float v = sqrtf(xv * xv + yv * yv);
			enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
			enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
			enemyBulletPosX[j] = posX;
			enemyBulletPosY[j] = posY;
			isEnemyBulletShot[j] = true;
			enemyBulletGHindex[j] = 2;
			break;
		}
	}
}

void EnemyAllDirection(float posX, float posY) {
	//弾の間の角度
	float angle = 15.f * (float)M_PI / 180.f;

	//左の弾を撃つ
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
			//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
			if (!isEnemyBulletShot[j]) {
				//プレイヤーに向かって移動するベクトルを計算

				float vx, vy;
				vx = playerPosX - posX;
				vy = playerPosY - posY;
				float xv = vx * cosf(i * angle) - vy * sinf(i * angle);
				float yv = vy * cosf(i * angle) + vx * sinf(i * angle);
				float v = sqrtf(xv * xv + yv * yv);
				enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
				enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
				enemyBulletPosX[j] = posX;
				enemyBulletPosY[j] = posY;
				isEnemyBulletShot[j] = true;
				enemyBulletGHindex[j] = 3;
				break;
			}
		}
	}
}

void BossSpiralStart() {
	if (!isSpiralActive) {
		isSpiralActive = true;
		spiralAngle = 0;
	}
}

void BossSpiralUpdate(float posX, float posY) {
	if (isSpiralActive) {
			for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
				//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
				if (!isEnemyBulletShot[j]) {
					//プレイヤーに向かって移動するベクトルを計算

					float vx, vy;
					vx = playerPosX - posX;
					vy = playerPosY - posY;
					float xv = vx * cosf(spiralAngle) - vy * sinf(spiralAngle);
					float yv = vy * cosf(spiralAngle) + vx * sinf(spiralAngle);
					float v = sqrtf(xv * xv + yv * yv);
					enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
					enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
					enemyBulletPosX[j] = posX;
					enemyBulletPosY[j] = posY;
					isEnemyBulletShot[j] = true;
					enemyBulletGHindex[j] = 4;
					break;
				}
			}
			spiralAngle += float(14.f * M_PI / 180.f);
			if (spiralAngle > 14 * M_PI) {
				isSpiralActive = false;
			}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 960, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//シーン切り替え
	enum Scene
	{
		TITLE,
		GAME,
		RESULT
	};

	Scene scene = TITLE;

	//画像読み込み
	int playerGH = Novice::LoadTexture("./images/player.png");                     //プレイヤー
	int playerInvincible = Novice::LoadTexture("./images/playerInvincible.png");   //プレイヤー（被弾時）
	int playerBulletGH = Novice::LoadTexture("./images/bullet.png");               //プレイヤーの弾
	int backgroundGH = Novice::LoadTexture("./images/background.png");             //アニメーション背景
	int backgroundRightGH = Novice::LoadTexture("./images/backgroundRight.png");   //画面右の背景
	int titleGH = Novice::LoadTexture("./images/title.png");                       //タイトル画面
	int triangleGH = Novice::LoadTexture("./images/triangle.png");               //タイトル・リザルトで使用する三角形
	int titleLetterGH = Novice::LoadTexture("./images/titleLetter.png");           //タイトル文字
	int gameClearGH = Novice::LoadTexture("./images/gameClear.png");               //ゲームクリア画面
	int gameOverGH = Novice::LoadTexture("./images/gameOver.png");                 //ゲームクリア画面
	int enemyGH = Novice::LoadTexture("./images/enemy.png");                       //敵
	int bossGH = Novice::LoadTexture("./images/boss.png");                         //ボス

	int enemyBulletGH[] = {
		Novice::LoadTexture("./images/enemyBullet0.png"),
		Novice::LoadTexture("./images/enemyBullet1.png"),
		Novice::LoadTexture("./images/enemyBullet2.png"),
		Novice::LoadTexture("./images/enemyBullet3.png"),
		Novice::LoadTexture("./images/enemyBullet4.png"),
	};

	int explodeGH = Novice::LoadTexture("./images/explode.png");                   //爆発
	int hpGH = Novice::LoadTexture("./images/hp.png");                             //ハート
	int numberGH[10] = {
		Novice::LoadTexture("./images/0.png"),
		Novice::LoadTexture("./images/1.png"),
		Novice::LoadTexture("./images/2.png"),
		Novice::LoadTexture("./images/3.png"),
		Novice::LoadTexture("./images/4.png"),
		Novice::LoadTexture("./images/5.png"),
		Novice::LoadTexture("./images/6.png"),
		Novice::LoadTexture("./images/7.png"),
		Novice::LoadTexture("./images/8.png"),
		Novice::LoadTexture("./images/9.png"),
	};

	// 乱数のシードを設定
	srand((unsigned)time(NULL));

	InitializeGameScene();

	//アニメーション背景のY座標
	int bgTopY = -720;       //上（画面外）にあるアニメーション背景
	int bgMidY = 0;          //下（画面内）にあるアニメーション背景

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		allTheTimeCount++;

		switch (scene)
		{
		case TITLE:
			//スペースキーを押したらゲーム開始
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				scene = GAME;
				InitializeGameScene();
			}
			gameCount++;
			break;

		case GAME:
			//経過フレームを増加
			gameCount++;

			//残り時間を1秒毎に減少
			if (gameCount % 60 == 0) {
				gameLeftTime--;
			}

			velX = 0;
			velY = 0;

			if (Novice::CheckHitKey(DIK_UP)) {
				velY -= playerSpd;
			}

			if (Novice::CheckHitKey(DIK_LEFT)) {
				velX -= playerSpd;
			}

			if (Novice::CheckHitKey(DIK_DOWN)) {
				velY += playerSpd;
			}

			if (Novice::CheckHitKey(DIK_RIGHT)) {
				velX += playerSpd;
			}

			//プレイヤーの斜め移動時の速度を調整
			if (velX && velY) {
				velX *= cosf((float)M_PI / 4.f);
				velY *= sinf((float)M_PI / 4.f);
			}

			playerPosX += velX;
			playerPosY += velY;

			//プレイヤーが画面外に出ないように画面端で座標を固定
			if (playerPosX < playerR) {
				playerPosX = playerR;
			}

			if (playerPosX > 660 - playerR) {
				playerPosX = 660 - playerR;
			}

			if (playerPosY < playerR) {
				playerPosY = playerR;
			}

			if (playerPosY > 720 - playerR) {
				playerPosY = 720 - playerR;
			}

			//シフトキーを押している間は速度が半分になる
			if (keys[DIK_LSHIFT]) {
				playerSpd = 3.0f;
			} else {
				playerSpd = 6.0f;
			}

			//Zキーを押しているかつ10ループに一回、プレイヤーが弾を発射
			if (keys[DIK_Z]) {
				if (gameCount % 5 == 0) {
					for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
						if (isPlayerBulletShot[i] == false) {
							isPlayerBulletShot[i] = true;
							playerBulletPosX[i] = playerPosX;
							playerBulletPosY[i] = playerPosY;
							break;
						}
					}
				}
			}


			//プレイヤーの弾を移動させる処理
			for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
				if (isPlayerBulletShot[i]) {
					playerBulletPosY[i] -= playerBulletSpd;
					//弾が画面外に出たらフラグをfalseにする
					if (playerBulletPosY[i] < 0) {
						isPlayerBulletShot[i] = false;
					}
				}
			}

			//プレイヤーの弾が敵に衝突した時の処理
			for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
				for (int j = 0; j < ENEMY_NUM; j++) {
					float x = playerBulletPosX[i] - enemyPosX[j];
					float y = playerBulletPosY[i] - enemyPosY[j];
					float r = playerBulletR + enemyR;
					if (x * x + y * y < r * r && isEnemyAlive[j] && isPlayerBulletShot[i]) {
						//プレイヤーの弾が敵に衝突した場合、衝突した弾を消して敵の体力を減少させる
						isPlayerBulletShot[i] = false;
						if (enemyHP[j] > 0) {
							enemyHP[j]--;
							//敵の体力が0になったら敵を消滅させる
							if (enemyHP[j] <= 0) {
								isEnemyAlive[j] = false;
								//プレイヤーのスコアを加算
								playerScore += 500;
							}
						}
						break; //衝突判定が発生した場合、内側のループを抜ける
					}
				}
			}

			//敵が消滅した際、敵消滅時のカウントを減らす（爆発画像表示用）
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (!isEnemyAlive[i]) {
					enemyDeadCount[i]--;
				}
			}

			//敵の移動処理
			for (int i = 0; i < ENEMY_NUM; i++) {
				enemyPosY[i] += enemySpd[i];
				//敵0のY座標が140に到達したら止まり、帰還カウントを減らす
				if (enemyPosY[i] == 140 || gameLeftTime <= 21) {
					enemySpd[i] = 0;
					enemyReturnCount[i]--;
					//帰還カウントが0になる、または20秒になったら敵0を帰還させる
					if (enemyReturnCount[i] == 0 || gameLeftTime <= 21) {
						enemySpd[i] = -enemyReturnSpd[i];
					}
				}
			}

			//敵の体力が0になった場合、敵の速度を0にして固定
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemyHP[i] == 0) {
					enemySpd[i] = 0;
					enemyReturnSpd[i] = 0;
				}
			}

			//敵が画面内に出現してから敵の保持カウントを増加させる
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemyPosY[i] > 0) {
					enemyTimeCount[i]++;
				}
			}

			//敵が弾を発射する処理
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (enemyTimeCount[i] % enemyRapidCount == 0) {
					if (isEnemyAlive[i] && enemyPosY[i] > 0) {
						//60~50秒
						if (gameLeftTime >= 50) {
							EnemyShot(enemyPosX[i], enemyPosY[i]);
						}
						//50秒~40秒
						else if (gameLeftTime >= 40) {
							//射撃間隔を早くする
							enemyRapidCount = 6;
							EnemyRandom(enemyPosX[i], enemyPosY[i]);
						}
						//40~30秒
						else if (gameLeftTime >= 30) {
							//射撃間隔を少し遅くする
							enemyRapidCount = 60;
							Enemy3Way(enemyPosX[i], enemyPosY[i]);
						}
						//30秒~20秒
						else if (gameLeftTime >= 20) {
							//射撃間隔を少し遅くする
							enemyRapidCount = 140;
							EnemyAllDirection(enemyPosX[i], enemyPosY[i]);
						}
					}
				}
			}

			//ボスの移動処理
			if (gameLeftTime <= 21 && bossPosY <= 140) {
				bossPosY += 1;
			}

			BossSpiralUpdate(bossPosX,bossPosY);
			if (gameLeftTime == 19) {
				BossSpiralStart();
			}

			if (gameLeftTime <= 16 && gameLeftTime >= 14 && gameCount % 25 == 0) {
				Enemy3Way(bossPosX, bossPosY);
			}

			if (gameLeftTime == 14) {
				BossSpiralStart();
			}


			if (gameLeftTime <= 9&&gameCount%60==0) {
				EnemyAllDirection(bossPosX,bossPosY);
			}

			if (gameLeftTime <= 9 && gameCount % 1 == 0) {
				EnemyRandom(bossPosX, bossPosY);
			}

			//プレイヤーの弾がボスに衝突した際の処理
			for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
					float x = playerBulletPosX[i] - bossPosX;
					float y = playerBulletPosY[i] - bossPosY;
					float r = playerBulletR + bossR;
					if (x * x + y * y < r * r && isBossAlive&& isPlayerBulletShot[i]) {
						//プレイヤーの弾が敵に衝突した場合、衝突した弾を消して敵の体力を減少させる
						playerScore += 100;
						isPlayerBulletShot[i] = false;
						if (bossHP > 0) {
							bossHP--;
							//敵の体力が0になったら敵を消滅させる
							if (bossHP <= 0) {
								isBossAlive = false;
							}
						}
						break; //衝突判定が発生した場合、内側のループを抜ける
					}
			}

			//敵の弾の移動処理
			for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
				if (isEnemyBulletShot[i]) {
					//プレイヤーに向かって移動させる
					enemyBulletPosX[i] += enemyBulletVecX[i];
					enemyBulletPosY[i] += enemyBulletVecY[i];
					//画面外に出たら削除
					if (enemyBulletPosX[i] > 660 || enemyBulletPosX[i] < 0 || enemyBulletPosY[i] > 720 || enemyBulletPosY[i] < 0) {
						isEnemyBulletShot[i] = false;
					}
				}
			}

			//敵の弾がプレイヤーに衝突した時の処理
			for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
				float x = enemyBulletPosX[i] - playerPosX;
				float y = enemyBulletPosY[i] - playerPosY;
				float r = enemyBulletR + (playerR - 12);
				if (x * x + y * y < r * r && isEnemyBulletShot[i] && !isPlayerInvincible) {
					//プレイヤーが敵の弾に衝突した場合、プレイヤーの体力を減少させてプレイヤーを無敵にする
					playerHP--;
					isPlayerInvincible = true;
					//無敵時間を120にする
					playerInvincibleCount = 120;
					break;
				}
			}

			//プレイヤーが無敵状態かつ無敵時間が残っている場合の処理
			if (isPlayerInvincible && playerInvincibleCount > 0) {
				//無敵時間のカウントを減らす
				playerInvincibleCount--;
				//無敵時間が終わった場合、無敵状態を解除する
				if (playerInvincibleCount <= 0) {
					isPlayerInvincible = false;
					playerInvincibleCount = 120;
				}
			}

			//アニメーション背景のY座標を増加
			bgTopY += 5;
			bgMidY += 5;
			if (bgTopY >= 0 && bgMidY >= 720) {
				bgTopY = -720;
				bgMidY = 0;
			}

			if (gameLeftTime <= 0 || playerHP <= 0) {
				scene = RESULT;
			}

			break;

		case RESULT:
			//スペースキーを押したらタイトルに戻る
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				scene = TITLE;
			}
			break;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		switch (scene)
		{
		case TITLE:
			//タイトル背景
			Novice::DrawSprite(
				0, 0,
				titleGH,
				1, 1,
				0.0f,
				0xFFFFFFFF
			);

			//タイトル画面の動く三角形
			Novice::DrawSprite(
				(int)(40 + sinf((float)gameCount * (float)M_PI * 4.0f / 180.f) * 20), 508,
				triangleGH,
				0.75f, 0.75f,
				0.0f,
				0xFFFFFFFF
			);

			//タイトル文字を上下移動
			Novice::DrawSprite(
				0, (int)(0+sinf((float)gameCount* (float)M_PI*2.0f/180.f)*30),
				titleLetterGH,
				1, 1,
				0.0f,
				0xFFFFFFFF
			);
			break;
		case GAME:
		{
			//ここからゲームの描画

			//アニメーション背景の描画
			Novice::DrawSprite(
				0, bgTopY,
				backgroundGH,
				1, 1,
				0.0f,
				0xFFFFFFFF
			);
			Novice::DrawSprite(
				0, bgMidY,
				backgroundGH,
				1, 1,
				0.0f,
				0xFFFFFFFF
			);

			//プレイヤーの描画
			if (!isPlayerInvincible) {
				Novice::DrawSprite(
					static_cast<int>(playerPosX) - static_cast<int>(playerR),   //左上X座標(半径を引いて中心位置を調整)
					static_cast<int>(playerPosY) - static_cast<int>(playerR),   //左上Y座標(半径を引いて中心位置を調整)
					playerGH,                                                   //ハンドル
					1,                                                          //X倍率
					1,                                                          //Y倍率                
					0.0f,                                                       //回転角
					0xFFFFFFFF                                                  //色
				);
			}

			//プレイヤー被弾時の描画
			if (isPlayerInvincible) {
				Novice::DrawSprite(
					static_cast<int>(playerPosX) - static_cast<int>(playerR),   //左上X座標(半径を引いて中心位置を調整)
					static_cast<int>(playerPosY) - static_cast<int>(playerR),   //左上Y座標(半径を引いて中心位置を調整)
					playerInvincible,                                                   //ハンドル
					1,                                                          //X倍率
					1,                                                          //Y倍率                
					0.0f,                                                       //回転角
					0xFFFFFFFF                                                  //色
				);
			}

			//発射した弾の描画
			for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
				if (isPlayerBulletShot[i]) {
					Novice::DrawSprite(
						static_cast<int>(playerBulletPosX[i]) - static_cast<int>(playerBulletR),   //左上X座標(半径を引いて中心位置を調整)
						static_cast<int>(playerBulletPosY[i]) - static_cast<int>(playerBulletR),   //左上Y座標(半径を引いて中心位置を調整)
						playerBulletGH,                                                            //ハンドル
						1,                                                                         //X倍率
						1,                                                                         //Y倍率
						0.0f,                                                                      //回転角
						0xFFFFFFFF                                                                 //色
					);
				}
			}

			//発射中の敵の弾を描画
			for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
				if (isEnemyBulletShot[i]) {
					Novice::DrawSprite(
						static_cast<int>(enemyBulletPosX[i]) - static_cast<int>(enemyBulletR),     //左上X座標(半径を引いて中心位置を調整)
						static_cast<int>(enemyBulletPosY[i]) - static_cast<int>(enemyBulletR),     //左上Y座標(半径を引いて中心位置を調整)
						enemyBulletGH[enemyBulletGHindex[i]],                                      //ハンドル
						1,                                                                         //X倍率
						1,                                                                         //Y倍率
						0.0f,                                                                      //回転角
						0xFFFFFFFF                                                                 //色
					);
				}
			}

			//生存している敵を描画
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (isEnemyAlive[i]) {
					Novice::DrawSprite(
						static_cast<int>(enemyPosX[i]) - static_cast<int>(enemyR),
						static_cast<int>(enemyPosY[i]) - static_cast<int>(enemyR),
						enemyGH,
						1,
						1,
						0.0f,
						0xFFFFFFFF
					);
				}
			}

			//敵が消滅した位置に爆発画像を描画
			for (int i = 0; i < ENEMY_NUM; i++) {
				if (!isEnemyAlive[i] && enemyDeadCount[i] > 0) {
					Novice::DrawSprite(
						static_cast<int>(enemyPosX[i]) - static_cast<int>(enemyR),
						static_cast<int>(enemyPosY[i]) - static_cast<int>(enemyR),
						explodeGH,
						1,
						1,
						0.0f,
						0xFFFFFFFF
					);
				}
			}

			//ボスの描画
			if (isBossAlive) {
				Novice::DrawSprite(
					static_cast<int>(bossPosX) - static_cast<int>(bossR),
					static_cast<int>(bossPosY) - static_cast<int>(bossR),
					bossGH,
					1,
					1,
					0.0f,
					0xFFFFFFFF
				);
			}

			//画面右の背景
			Novice::DrawSprite(
				660, 0,
				backgroundRightGH,
				1, 1,
				0.0f,
				0xFFFFFFFF
			);

			//体力の表示
			for (int i = 0; i < playerHP; i++) {
				Novice::DrawSprite(
					810 + (i * 45), 158,
					hpGH,
					1, 1,
					0.0f,
					0xFFFFFFFF
				);
			}

			//残り時間の表示
			int onesTimeDigit, tensTimeDigit;

			onesTimeDigit = (gameLeftTime % 10);
			tensTimeDigit = (gameLeftTime / 10);

			//10の位を表示
			Novice::DrawSprite(
				800, 28,
				numberGH[tensTimeDigit],
				1, 1,
				0.0f,
				0xFFFFFFFF
			);

			//1の位を表示
			Novice::DrawSprite(
				832, 28,
				numberGH[onesTimeDigit],
				1, 1,
				0.0f,
				0xFFFFFFFF
			);

			//スコアの表示
			int scoreDigit[5];
			int scoreTemp = playerScore;

			scoreDigit[0] = (scoreTemp % 10);
			scoreTemp /= 10;
			scoreDigit[1] = (scoreTemp % 10);
			scoreTemp /= 10;
			scoreDigit[2] = (scoreTemp % 10);
			scoreTemp /= 10;
			scoreDigit[3] = (scoreTemp % 10);
			scoreTemp /= 10;
			scoreDigit[4] = (scoreTemp % 10);

			//10000~1の位を表示
			for (int i = 0; i < 5; i++) {
				Novice::DrawSprite(
					800 + (i * 32), 92,
					numberGH[scoreDigit[4 - i]],
					1, 1,
					0.0f,
					0xFFFFFFFF
				);
			}

			break;
		}
		case RESULT:
			//ゲームクリアの場合
			if (gameLeftTime <= 0) {
				Novice::DrawSprite(
					0, 0,
					gameClearGH,
					1, 1,
					0.0f,
					0xFFFFFFFF
				);

				//ゲームクリア画面の動く三角形
				Novice::DrawSprite(
					(int)(40 + sinf((float)allTheTimeCount * (float)M_PI * 4.0f / 180.f) * 20), 508,
					triangleGH,
					0.75f, 0.75f,
					0.0f,
					0xFFFFFFFF
				);
			}
			//ゲームオーバーの場合
			if (playerHP <= 0) {
				Novice::DrawSprite(
					0, 0,
					gameOverGH,
					1, 1,
					0.0f,
					0xFFFFFFFF
				);
			}

			//ゲームオーバー画面の動く三角形
			Novice::DrawSprite(
				(int)(40 + sinf((float)allTheTimeCount * (float)M_PI * 4.0f / 180.f) * 20), 508,
				triangleGH,
				0.75f, 0.75f,
				0.0f,
				0xFFFFFFFF
			);

			break;
		}

		///
		/// ↑描画処理ここまで
		///

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