#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PLAYER_BULLET_NUM 20  //プレイヤーが発射する弾の最大数
#define ENEMY_NUM 10          //敵の最大数
#define ENEMY_BULLET_NUM 100   //敵が発射する弾の最大数

const char kWindowTitle[] = "shooting";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 960, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//画像読み込み
	int playerGH = Novice::LoadTexture("./images/player.png");           //プレイヤー画像
	int playerBulletGH = Novice::LoadTexture("./images/bullet.png");     //プレイヤーの弾画像
	int backgroundGH = Novice::LoadTexture("./images/background.png");   //アニメーション背景画像
	int enemyGH = Novice::LoadTexture("./images/enemy.png");             //敵画像
	int enemyBulletGH = Novice::LoadTexture("./images/enemyBullet.png");     //敵の弾画像

	//プレイヤーの情報
	float playerPosX = 330.0f;   //X座標
	float playerPosY = 640.0f;   //Y座標
	float playerR = 16.0f;       //半径
	float playerSpd = 6.0f;      //速度
	int playerScore = 0;         //現在のスコア

	//プレイヤーの弾の情報
	float playerBulletPosX[PLAYER_BULLET_NUM];          //X座標
	float playerBulletPosY[PLAYER_BULLET_NUM];          //Y座標
	bool isPlayerBulletShot[PLAYER_BULLET_NUM];         //プレイヤーの弾が撃たれているかのフラグ
	float playerBulletR = 8.0f;                         //半径
	float playerBulletSpd = 24.0f;                      //速度

	//プレイヤーの弾の情報の配列を初期化
	for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
		playerBulletPosX[i] = 0.0f;
		playerBulletPosY[i] = 0.0f;
		isPlayerBulletShot[i] = false;
	}

	//敵の座標を決める為の乱数の最小値と最大値
	int lower = 20;
	int upper = 640;

	// 乱数のシードを設定
	srand((unsigned)time(NULL));

	//敵の情報
	float enemyPosX[ENEMY_NUM];      //X座標
	float enemyPosY[ENEMY_NUM];      //Y座標
	bool isEnemyAlive[ENEMY_NUM];    //敵が生存しているかのフラグ
	float enemyR = 16.0f;            //半径
	int enemysLeft = ENEMY_NUM;      //敵の残りカウント

	//敵の情報の配列を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		//敵のX座標を決めるための乱数（20~640）を生成
		int random_number = (rand() % (upper - lower + 1)) + lower;
		enemyPosX[i] = (float)random_number;
		//Y座標を80ずつ上にずらして生成
		enemyPosY[i] = -80.0f * i;
		isEnemyAlive[i] = true;
	}

	//敵の弾の情報
	float enemyBulletPosX[ENEMY_BULLET_NUM];  //X座標
	float enemyBulletPosY[ENEMY_BULLET_NUM];  //Y座標
	bool isEnemyBulletShot[ENEMY_BULLET_NUM]; //弾が撃たれているかのフラグ
	float enemyBulletR = 12.0f;               //半径
	float enemyBulletSpd = 4.0f;              //速度

	//敵の弾の情報を初期化
	for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
		enemyBulletPosX[i] = 0.0f;
		enemyBulletPosY[i] = 0.0f;
		isEnemyBulletShot[i] = false;
	}

	//アニメーション背景のY座標
	int bgTopY = -720;       //上（画面外）にあるアニメーション背景
	int bgMidY = 0;          //下（画面内）にあるアニメーション背景

	//ゲームの経過フレームをカウント
	long long gameCount = 0;

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

		//プレイヤーのキー移動
		float velX = 0;
		float velY = 0;

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

		//Zキーを押しているかつ6ループに一回、プレイヤーが弾を発射
		if (keys[DIK_Z]) {
			if (gameCount % 6 == 0) {
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

		//敵を下方向に移動させる
		for (int i = 0; i < ENEMY_NUM; i++) {
			enemyPosY[i] += 1;
		}

		//プレイヤーの弾が敵に衝突した時の処理
		for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
			for (int j = 0; j < ENEMY_NUM; j++) {
				float x = playerBulletPosX[i] - enemyPosX[j];
				float y = playerBulletPosY[i] - enemyPosY[j];
				float r = playerBulletR + enemyR;
				if (x * x + y * y < r * r) {
					//プレイヤーの弾が敵に衝突した場合、衝突した弾を消して敵を消滅させる
					if (isEnemyAlive[j]) {
						isPlayerBulletShot[i] = false;
						//敵の体力が0になったら敵を消滅させる
						isEnemyAlive[j] = false;
						//敵を倒す度にスコアを加算
						playerScore += 500;
						//敵を倒す度に敵の残りカウントを減らす
						enemysLeft--;
					}
				}
			}
		}

		//敵が弾を発射する処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
				//60フレームに1発
				if (gameCount % 60 == 0) {
					//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
					if (!isEnemyBulletShot[j] && isEnemyAlive[i] && enemyPosY[i] > 0) {
						enemyBulletPosX[j] = enemyPosX[i];
						enemyBulletPosY[j] = enemyPosY[i];
						isEnemyBulletShot[j] = true;
						break;
					}
				}
			}
		}

		//敵の弾の移動処理
		for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
			if (isEnemyBulletShot[i]) {
				//Y方向に速度を加算している
				enemyBulletPosY[i] += enemyBulletSpd;
				//敵の弾が画面外に出たらフラグをfalseに
				if (enemyBulletPosY[i] > 720) {
					isEnemyBulletShot[i] = false;
				}
			}
		}

		//アニメーション背景のY座標を増加
		bgTopY += 5;
		bgMidY += 5;
		if (bgTopY >= 0 && bgMidY >= 720) {
			bgTopY = -720;
			bgMidY = 0;
		}

		//ゲームカウントを増加（仮置き、後でシーン切り替えする時に開始に変更する）
		gameCount++;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//デバッグ用文字列
		Novice::ScreenPrintf(700, 640, "%.1f,%.1f,", playerPosX, playerPosY); //自機の座標
		Novice::ScreenPrintf(700, 660, "%.1f,%.1f", velX, velY);              //自機の速度
		Novice::ScreenPrintf(700, 680, "%d", gameCount);                      //経過フレーム
		Novice::ScreenPrintf(700, 40, "score:%d", playerScore);               //スコアを表示
		Novice::ScreenPrintf(700, 80, "enemys left:%d", enemysLeft);          //残りの敵の数

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
		Novice::DrawSprite(
			static_cast<int>(playerPosX) - static_cast<int>(playerR),   //左上X座標(半径を引いて中心位置を調整)
			static_cast<int>(playerPosY) - static_cast<int>(playerR),   //左上Y座標(半径を引いて中心位置を調整)
			playerGH,                                                   //ハンドル
			1,                                                          //X倍率
			1,                                                          //Y倍率                
			0.0f,                                                       //回転角
			0xFFFFFFFF                                                  //色
		);

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
					enemyBulletGH,                                                             //ハンドル
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