#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 960, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//画像読み込み
	int playerGH = Novice::LoadTexture("./images/player.png"); //プレイヤー画像
	int enemyGH = Novice::LoadTexture("./images/enemy.png");   //敵画像
	int bulletGH = Novice::LoadTexture("./images/bullet.png"); //弾画像

	//プレイヤーの情報
	float playerPosX = 340.0f;   //X座標
	float playerPosY = 640.0f;   //Y座標
	float playerR = 16.0f;       //半径
	float playerSpd = 6.0f;      //速度

	//敵の情報
	float enemyPosX = 340.0f;    //X座標
	float enemyPosY = 80.0f;     //Y座標
	float enemyR = 16.0f;        //半径
	bool isEnemyAlive = true;    //生存しているかのフラグ

	//弾の情報
	float bulletPosX = 0.0f;     //X座標
	float bulletPosY = 0.0f;     //Y座標
	float bulletR = 8.0f;        //半径
	float bulletSpd = 16.0f;     //速度
	bool isBulletShot = false;   //弾が撃たれているかのフラグ

	//敵と弾の距離
	float e2bR = enemyR + bulletR;

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

		if (Novice::CheckHitKey(DIK_W)) {
			velY -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_A)) {
			velX -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_S)) {
			velY += playerSpd;
		}

		if (Novice::CheckHitKey(DIK_D)) {
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
		if (playerPosX < 40 + playerR) {    
			playerPosX = 40 + playerR;
		}

		if (playerPosX > 640 - playerR) {    
			playerPosX = 640 - playerR;
		}

		if (playerPosY < 20 + playerR) {
			playerPosY = 20 + playerR;
		}

		if (playerPosY > 700 - playerR) {
			playerPosY = 700 - playerR;
		}

		//スペースキーを押した瞬間に弾を発射
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			if (!isBulletShot) {
				isBulletShot = true;
				bulletPosX = playerPosX;
				bulletPosY = playerPosY;
			}
		}

		//弾の移動と画面外に出た際の処理
		if (isBulletShot) {
			bulletPosY -= bulletSpd;
			if (bulletPosY < 20) {
				isBulletShot = false;
			}
		}

		//弾と敵の中心間の距離を求める
		float difX = enemyPosX - bulletPosX;
		float difY = enemyPosY - bulletPosY;
		float diff = difX * difX + difY * difY;

		//弾が敵に衝突した場合、敵の生存フラグをfalseに変更
		if (diff <= e2bR * e2bR) {
			isEnemyAlive = false;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//ゲーム背景（デバッグ用）
		Novice::DrawBox(
			40,              //左上X座標
			20,              //左上Y座標
			600,             //横幅
			680,             //縦幅
			0.0f,            //回転角
			0x444444FF,      //色
			kFillModeSolid   //塗りつぶし
		);

		//弾の発射中、弾を描画
		if (isBulletShot) {
			Novice::DrawSprite(
				static_cast<int>(bulletPosX) - static_cast<int>(bulletR),   //左上X座標(半径を引いて中心位置を調整)
				static_cast<int>(bulletPosY) - static_cast<int>(bulletR),   //左上Y座標(半径を引いて中心位置を調整)
				bulletGH,                                                   //ハンドル
				1,                                                          //X倍率
				1,                                                          //Y倍率
				0.0f,                                                       //回転角
				0xFFFFFFFF                                                  //色
			);
		}

		//自機を描画
		Novice::DrawSprite(
			static_cast<int>(playerPosX) - static_cast<int>(playerR),   //左上X座標(半径を引いて中心位置を調整)
			static_cast<int>(playerPosY) - static_cast<int>(playerR),   //左上Y座標(半径を引いて中心位置を調整)
			playerGH,                                                   //ハンドル
			1,                                                          //X倍率
			1,                                                          //Y倍率                
			0.0f,                                                       //回転角
			0xFFFFFFFF                                                  //色
		);

		//敵が生存中、敵を描画
		if (isEnemyAlive) {
			Novice::DrawSprite(
				static_cast<int>(enemyPosX) - static_cast<int>(enemyR),   //左上X座標(半径を引いて中心位置を調整)
				static_cast<int>(enemyPosY) - static_cast<int>(enemyR),   //左上Y座標(半径を引いて中心位置を調整)
				enemyGH,                                                  //ハンドル
				1,                                                        //X倍率
				1,                                                        //Y倍率                
				0.0f,                                                     //回転角
				0xFFFFFFFF                                                //色
			);
		}

		//自機の座標文字列を表示（デバッグ用）
		Novice::ScreenPrintf(700, 640, "%.1f,%.1f,", playerPosX, playerPosY);

		//自機の速度文字列を表示（デバッグ用）
		Novice::ScreenPrintf(700, 660, "%.1f,%.1f", velX, velY);

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
