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

	//サウンドの読み込み
	int fanfareBH = Novice::LoadAudio("./Sounds/fanfare.wav");
	int mokugyoBH = Novice::LoadAudio("./Sounds/mokugyo.wav");
	int startBH = Novice::LoadAudio("./Sounds/start.wav");

	Scene scene = TITLE;
	Novice::PlayAudio(fanfareBH, true, 0.1f);

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

		//シーン毎の背景描画と音の再生
		switch (scene) {
		case TITLE:
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0xD86666FF,
				kFillModeSolid
			);
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				Novice::StopAudio(startBH); 
				Novice::PlayAudio(fanfareBH, true, 0.1f);
			}
			break;
		case GAME:
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0x66D866FF,
				kFillModeSolid
			);
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				Novice::StopAudio(fanfareBH);
				Novice::PlayAudio(mokugyoBH, true, 0.1f);
			}
			break;
		case RESULT:
			Novice::DrawBox(
				0, 0,
				1280, 720,
				0.0f,
				0x6666D8FF,
				kFillModeSolid
			);
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				Novice::StopAudio(mokugyoBH);
				Novice::PlayAudio(startBH, true, 0.1f);
			}
			break;
		}

		//シーン切り替えのキーとBGMの番号を表す文字列
		Novice::ScreenPrintf(100, 360, "SceneNo: %d   Push Space to change scene", scene);
		Novice::ScreenPrintf(100, 390, "is playing sound %d", Novice::IsPlayingAudio(fanfareBH));

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
