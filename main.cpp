#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PLAYER_BULLET_NUM 20   //プレイヤーが発射する弾の最大数
#define ENEMY_NUM 100          //敵の最大数
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
	int playerGH = Novice::LoadTexture("./images/player.png");                     //プレイヤー
	int playerInvincible = Novice::LoadTexture("./images/playerInvincible.png");   //プレイヤー（被弾時）
	int playerBulletGH = Novice::LoadTexture("./images/bullet.png");               //プレイヤーの弾
	int backgroundGH = Novice::LoadTexture("./images/background.png");             //アニメーション背景
	int enemyGH = Novice::LoadTexture("./images/enemy.png");                       //敵
	int enemyBulletGH = Novice::LoadTexture("./images/enemyBullet.png");           //敵の弾
	int explodeGH = Novice::LoadTexture("./images/explode.png");                   //爆発

	//プレイヤーの情報
	float playerPosX = 330.0f;         //X座標
	float playerPosY = 640.0f;         //Y座標
	float playerR = 16.0f;             //半径
	float playerSpd = 6.0f;            //速度
	int playerScore = 0;               //現在のスコア
	int playerHP = 3;                  //プレイヤーの体力
	bool isPlayerInvincible = false;   //プレイヤーが無敵状態かのフラグ
	int playerInvincibleCount = 0;     //プレイヤーの無敵時間カウント

	//プレイヤーの弾の情報
	float playerBulletPosX[PLAYER_BULLET_NUM];          //X座標
	float playerBulletPosY[PLAYER_BULLET_NUM];          //Y座標
	bool isPlayerBulletShot[PLAYER_BULLET_NUM];         //プレイヤーの弾が撃たれているかのフラグ
	float playerBulletR = 8.0f;                         //半径
	float playerBulletSpd = 32.0f;                      //速度

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
	float enemySpd[ENEMY_NUM];       //速度
	int enemyDeadCount[ENEMY_NUM];   //敵消滅時のカウント
	int enemyHP[ENEMY_NUM];          //敵の体力
	int enemyRapidCount = 60;        //敵の射撃間隔
	int enemyReturnCount[ENEMY_NUM]; //帰還カウント

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
		enemyReturnCount[i] = 120;
	}

	//敵の弾の情報
	float enemyBulletPosX[ENEMY_BULLET_NUM];  //X座標
	float enemyBulletPosY[ENEMY_BULLET_NUM];  //Y座標
	bool isEnemyBulletShot[ENEMY_BULLET_NUM]; //弾が撃たれているかのフラグ
	float enemyBulletR = 12.0f;               //半径
	float enemyBulletSpd = 5.0f;              //速度
	float enemyBulletVecX[ENEMY_BULLET_NUM];  //X方向のベクトル
	float enemyBulletVecY[ENEMY_BULLET_NUM];  //Y方向のベクトル

	//敵の弾の情報を初期化
	for (int i = 0; i < ENEMY_BULLET_NUM; i++) {
		enemyBulletPosX[i] = 0.0f;
		enemyBulletPosY[i] = 0.0f;
		isEnemyBulletShot[i] = false;
		enemyBulletVecX[i] = 0.0f;
		enemyBulletVecY[i] = 0.0f;
	}

	//アニメーション背景のY座標
	int bgTopY = -720;       //上（画面外）にあるアニメーション背景
	int bgMidY = 0;          //下（画面内）にあるアニメーション背景

	//ゲームの経過フレームをカウント
	long long gameCount = 0;

	//ゲームの残り時間
	int gameLeftTime = 60;

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

		//経過フレームを増加
		gameCount++;

		//残り時間を1秒毎に減少
		if (gameCount % 60 == 0) {
			gameLeftTime--;
		}

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

		for (int i = 0; i < ENEMY_NUM; i++) {
			enemyPosY[i] += enemySpd[i];
			//敵0のY座標が140に到達したら止まり、帰還カウントを減らす
			if (enemyPosY[i] == 140) {
				enemySpd[i] = 0;
				enemyReturnCount[i]--;
				//帰還カウントが0になったら、敵0を帰還させる
				if (enemyReturnCount[i] == 0) {
					enemySpd[i] = -4.0f;
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

		//敵が弾を発射する処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			for (int j = 0; j < ENEMY_BULLET_NUM; j++) {
				//敵の射撃間隔毎に1発
				if (gameCount % enemyRapidCount == 0) {
					//敵の弾が撃たれていないかつ敵が存在しているかつ敵が画面内にいる場合のみ弾を発射
					if (!isEnemyBulletShot[j] && isEnemyAlive[i] && enemyPosY[i] > 0) {
						//プレイヤーに向かって移動するベクトルを計算
						float xv = playerPosX - enemyPosX[i];
						float yv = playerPosY - enemyPosY[i];
						float v = sqrtf(xv * xv + yv * yv);
						enemyBulletVecX[j] = (xv / v) * enemyBulletSpd;
						enemyBulletVecY[j] = (yv / v) * enemyBulletSpd;
						enemyBulletPosX[j] = enemyPosX[i];
						enemyBulletPosY[j] = enemyPosY[i];
						isEnemyBulletShot[j] = true;
						break;
					}
				}
			}
		}

		//残り時間で敵の射撃間隔を短くする
		if (gameLeftTime < 50) {
			enemyRapidCount = 50;
		}

		if (gameLeftTime < 40) {
			enemyRapidCount = 40;
		}

		if (gameLeftTime < 30) {
			enemyRapidCount = 30;
		}

		if (gameLeftTime < 20) {
			enemyRapidCount = 20;
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
			float r = enemyBulletR + (playerR - 10);
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
		Novice::ScreenPrintf(700, 300, "left time:%d", gameLeftTime);         //残り時間
		Novice::ScreenPrintf(700, 40, "score:%d", playerScore);               //スコアを表示
		Novice::ScreenPrintf(700, 120, "playerHP:%d", playerHP);              //プレイヤーの体力

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