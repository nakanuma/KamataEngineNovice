#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//画像読み込み
	int playerGH = Novice::LoadTexture("./images/player.png"); 

	//プレイヤーの情報
	float playerPosX = 640.0f;   //X座標
	float playerPosY = 360.0f;   //Y座標
	float playerR = 16.0f;       //半径
	float playerSpd = 6.0f;      //速度
						    
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
		if (playerPosX < playerR) {
			playerPosX = playerR;
		}

		if (playerPosX > 1280 - playerR) {
			playerPosX = 1280 - playerR;
		}

		if (playerPosY < playerR) {
			playerPosY = playerR;
		}

		if (playerPosY > 720 - playerR) {
			playerPosY = 720 - playerR;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//自機を描画
		Novice::DrawSprite(
			(int)playerPosX - (int)playerR,
			(int)playerPosY - (int)playerR,
			playerGH,
			1, 1,
			0.0f,
			0xFFFFFFFF
		);

		//自機の座標を表示
		Novice::ScreenPrintf(20, 20, "%.1f,%.1f,", playerPosX, playerPosY);

		//自機の速度を表示
		Novice::ScreenPrintf(20, 40, "%.1f,%.1f", velX, velY);

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
