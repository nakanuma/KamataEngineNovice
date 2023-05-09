#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//画像読み込み
	int bgGH = Novice::LoadTexture("./images/bg.png");

	//シーンを管理する
	enum Scene {
		TITLE,
		GAME,
		RESULT
	};

	//アニメーション背景のY座標
	int bg1Y = 0;
	int bg2Y = -720;

	Scene scene = TITLE;

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

		//シーン毎の更新処理
		switch (scene) {
		case TITLE:
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				scene = GAME;
			}
			break;
		case GAME:
			bg2Y += 5;
			bg1Y += 5;
			if (bg2Y >= 0 && bg1Y >= 720) {
				bg2Y = -720;
				bg1Y = 0;
			}
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				scene = RESULT;
				}
			break;
		case RESULT:
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

		//シーン毎の描画
		switch (scene) {
		case TITLE:
			Novice::DrawBox(
				0, 0, 
				1280, 720, 
				0.0f, 
				0xEFC1C1FF, 
				kFillModeSolid
			);
			break;
		case GAME:
			Novice::DrawSprite(
				0, bg2Y, 
				bgGH, 
				1, 1, 
				0.0f, 
				0xFFFFFFF
			);
			Novice::DrawSprite(
				0, bg1Y,
				bgGH,
				1, 1,
				0.0f,
				0xFFFFFFF
			);
			break;
		case RESULT:
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0xC1EFC1FF,
				kFillModeSolid
			);
			break;
		}

		//シーンナンバーと切り替えのキーを表す文字列
		Novice::ScreenPrintf(20, 300, "SceneNo = %d",scene);
		Novice::ScreenPrintf(20, 320, "Push Space to change scene");

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
