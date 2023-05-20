#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define PLAYER_BULLET_NUM 20  //プレイヤーが発射する弾の最大数

const char kWindowTitle[] = "shooting";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 960, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//画像読み込み
	int playerGH = Novice::LoadTexture("./images/player.png");         //プレイヤー画像
	int bulletGH = Novice::LoadTexture("./images/bullet.png");         //弾画像
	int backgroundGH = Novice::LoadTexture("./images/background.png"); //背景画像
	int enemyGH[] = {                                                  //敵の画像0~2まで
		Novice::LoadTexture("./images/enemy0.png"),
	    Novice::LoadTexture("./images/enemy1.png"),
	    Novice::LoadTexture("./images/enemy2.png"),
		Novice::LoadTexture("./images/enemy1.png"),
	};

	//プレイヤーの情報
	float playerPosX = 330.0f;   //X座標
	float playerPosY = 640.0f;   //Y座標
	float playerR = 16.0f;       //半径
	float playerSpd = 6.0f;      //速度

	//弾の情報
	float bulletPosX[PLAYER_BULLET_NUM];          //X座標
	float bulletPosY[PLAYER_BULLET_NUM];          //Y座標
	bool isPlayerBulletShot[PLAYER_BULLET_NUM];   //弾が撃たれているかのフラグ
	float bulletR = 8.0f;                         //半径
	float bulletSpd = 24.0f;                      //速度

	//弾の情報の配列を初期化
	for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
		bulletPosX[i] = 0.0f;
		bulletPosY[i] = 0.0f;
		isPlayerBulletShot[i] = false;
	}

	//敵の情報
	float enemyPosX = 50.0f;    //X座標
	float enemyPosY = -50.0f;   //Y座標
	float enemyR = 16.0f;       //半径
	int in_time = 180;          //敵が出てくるフレーム
	int stop_time = 300;        //敵が止まるフレーム
	int out_time = 420;         //敵が帰還するフレーム
	bool isEnemyReturn = false; //敵が帰還し終わったかのフラグ

	//アニメーション背景のY座標
	int bgTopY = -720;
	int bgMidY = 0;

	//ゲームの経過フレームをカウント（仮置き、後でシーン切り替えする時に開始に変更する）
	long long gameCount = 0;

	//敵のアニメーションタイマー
	int enemyAnimationTimer = 0;

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

		//Zキーを押しているかつ6ループに一回、弾を発射
		if (keys[DIK_Z]) {
			if (gameCount % 6 == 0) {
				for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
					if (isPlayerBulletShot[i] == false) {
						isPlayerBulletShot[i] = true;
						bulletPosX[i] = playerPosX;
						bulletPosY[i] = playerPosY;
						break;
					}
				}
			}
		}

		//弾を移動させる処理
		for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
			if (isPlayerBulletShot[i]) {
				bulletPosY[i] -= bulletSpd;
				//弾が画面外に出たらフラグをfalseにする
				if (bulletPosY[i] < 0) {
					isPlayerBulletShot[i] = false;
				}
			}
		}

		//10フレーム毎に敵のアニメーションタイマーをインクリメント
		if (gameCount % 10 == 0) {
			enemyAnimationTimer++;

			//3を超えたら0にする
			if (enemyAnimationTimer >= 4) {
				enemyAnimationTimer = 0;
			}
		}

		//敵が出てきてから止まるまでの間、下に移動
		if (in_time < gameCount && stop_time > gameCount) {
			enemyPosY += 2;
			//帰還時間を過ぎたら戻る
		} else if (gameCount > out_time) {
			enemyPosY -= 2;
			if (enemyPosY < -40) {
				isEnemyReturn = true;
			}
		}

		//アニメーション背景のY座標を増加
		bgTopY += 5;
		bgMidY += 5;
		if (bgTopY >= 0 && bgMidY >= 720) {
			bgTopY = -720;
			bgMidY = 0;
		}

		//ゲームカウントを増加
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
		Novice::ScreenPrintf(700, 700, "%f:%f", enemyPosX, enemyPosY);             //敵の座標

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

		//敵の描画
		if (!isEnemyReturn) {
			Novice::DrawSprite(
				static_cast<int>(enemyPosX) - static_cast<int>(enemyR),
				static_cast<int>(enemyPosY) - static_cast<int>(enemyR),
				enemyGH[enemyAnimationTimer],
				1,
				1,
				0.0f,
				0xFFFFFFFF
			);
		}

		//発射した弾の描画
		for (int i = 0; i < PLAYER_BULLET_NUM; i++) {
			if (isPlayerBulletShot[i]) {
				Novice::DrawSprite(
					static_cast<int>(bulletPosX[i]) - static_cast<int>(bulletR),   //左上X座標(半径を引いて中心位置を調整)
					static_cast<int>(bulletPosY[i]) - static_cast<int>(bulletR),   //左上Y座標(半径を引いて中心位置を調整)
					bulletGH,                                                      //ハンドル
					1,                                                             //X倍率
					1,                                                             //Y倍率
					0.0f,                                                          //回転角
					0xFFFFFFFF                                                     //色
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
