#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//円の初期位置
	float posX = 640.0f;
	float posY = 360.0f;

	//円の半径
	float radius = 32.0f;

	//円の速度
	float velX = 4.0f;
	float velY = 4.0f;

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

		//画面端で跳ね返る処理
		if (posX < radius || posX > 1280 - radius) {
			velX = -velX;
		}

		if (posY < radius || posY > 720 - radius) {
			velY = -velY;
		}

		//座標の更新
		posX += velX * 0.7f;
		posY += velY * 0.7f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//円の描画
		Novice::DrawEllipse(
			(int)posX,        //X座標
			(int)posY,        //Y座標
			(int)radius,      //X半径
			(int)radius,      //Y半径
			0.0f,             //回転角
			WHITE,            //色
			kFillModeSolid    //塗りつぶし
		);

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
