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

	//円の初期位置と半径
	float posX = 640.0f;
	float posY = 360.0f;
	float radius = 32.0f;

	//白い矩形の速さ
	float playerSpd = 2.0f;

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

		//白い矩形のキー移動
		float speedX = 0;
		float speedY = 0;

		if (Novice::CheckHitKey(DIK_A)|| Novice::CheckHitKey(DIK_LEFT)) {
			speedX -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_D)|| Novice::CheckHitKey(DIK_RIGHT)) {
			speedX += playerSpd;
		}

		if (Novice::CheckHitKey(DIK_W)|| Novice::CheckHitKey(DIK_UP)) {
			speedY -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_S)|| Novice::CheckHitKey(DIK_DOWN)) {
			speedY += playerSpd;
		}

		//斜め移動時の速度を調整
		if (speedX && speedY) {
			speedX *= 0.7f;
			speedY *= 0.7f;
		}

		posX += speedX;
		posY += speedY;

		//画面外に出ないように画面端で座標を固定
		if (posX < radius) {
			posX = radius;
		}

		if (posX > 1280 - radius) {
			posX = 1280 - radius;
		}

		if (posY < radius) {
			posY = radius;
		}

		if (posY > 720 - radius) {
			posY = 720 - radius;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//自機を描画
		Novice::DrawEllipse((int)posX, (int)posY, (int)radius, (int)radius, 0.0f, WHITE, kFillModeSolid);

		//キー操作を表す文字列
		Novice::ScreenPrintf(20, 20, "W||ArrowUp:Up A||ArrowLeft:Left S||ArrowDown:Down D||ArrowRight:Right");

		//自機の座標を表示
		Novice::ScreenPrintf(20, 60, "  posX:%7.1f /   posY:%7.1f", posX, posY);

		//自機の速度を表示
		Novice::ScreenPrintf(20, 80, "speedX:%7.1f / speedY:%7.1f", speedX, speedY);

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
