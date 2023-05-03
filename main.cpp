#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//シーンを管理する
	enum Scene {
		TITLE,
		GAME,
		RESULT
	};

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
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0xC1EFC1FF,
				kFillModeSolid
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
