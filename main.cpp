#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_AL1_12_確認課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	struct Vector2 {
		float x;
		float y;
	};

	//プレイヤーの構造体
	struct Player {
		Vector2 pos;
		float size;
	};

	//カメラの構造体
	struct Camera {
		Vector2 pos;
		Vector2 WorldToScreen(Vector2 world) {
			return { world.x - pos.x,world.y - pos.y };
		};
	};

	//プレイヤーの情報
	Player player{};
	player.pos.x = 128.0f;
	player.pos.y = 568.0f;
	player.size = 24.0f;

	//カメラの情報を0で初期化
	Camera camera{};

	//赤い線の情報
	float lineX = 800.0f;

	//画像読み込み
	int bgGH[] = {
		Novice::LoadTexture("./Resources/images/bg1.png"),
		Novice::LoadTexture("./Resources/images/bg2.png"),
		Novice::LoadTexture("./Resources/images/bg3.png"),
		Novice::LoadTexture("./Resources/images/bg4.png"),
	};

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

		//プレイヤーの移動処理
		if (keys[DIK_D]) {
			player.pos.x += 8;
		}

		if (keys[DIK_A]) {
			player.pos.x -= 8;
		}

		//プレイヤーが画面外へ行かない処理
		if (player.pos.x + player.size > 1280 * 4) {
			player.pos.x = 1280 * 4 - player.size;
		}

		//プレイヤーの座標をスクリーン座標に変換
		Vector2 playerScreen = camera.WorldToScreen(player.pos);

		//カメラの移動処理
		if (keys[DIK_D]) {
			if (playerScreen.x + player.size - 8 >= lineX) {
				camera.pos.x += 8;
			}
		}

		if (keys[DIK_A]) {
			if (playerScreen.x + player.size - 8 <= lineX) {
				camera.pos.x -= 8;
			}
		}

		//カメラが範囲外に行かない処理
		if (camera.pos.x < 0) {
			camera.pos.x = 0;
		}

		if (camera.pos.x > 1280 * 3) {
			camera.pos.x = 1280 * 3;
		}

		playerScreen = camera.WorldToScreen(player.pos);

		//赤い線の移動処理
		if (camera.pos.x < 1280 * 3) {
			if (keys[DIK_RIGHT]) {
				lineX += 8;
			}

			if (keys[DIK_LEFT]) {
				lineX -= 8;
			}

			//赤い線が画面外に行かないようにする
			if (lineX > 1280) {
				lineX = 1280;
			}

			if (lineX < playerScreen.x + player.size) {
				lineX = playerScreen.x + player.size;
			}
		}

		//背景の座標を計算
		Vector2 bgScreen = camera.WorldToScreen({ 0.0f,0.0f });

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//背景を描画
		for (int i = 0; i < 4; i++) {
			Novice::DrawSprite(
				(int)bgScreen.x + 1280 * i, (int)bgScreen.y,
				bgGH[i],
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);
		}

		//プレイヤーの描画
		Novice::DrawBox(
			(int)playerScreen.x, (int)playerScreen.y,
			(int)player.size, (int)player.size,
			0.0f,
			WHITE,
			kFillModeSolid
		);

		//赤い線を描画
		Novice::DrawLine(
			(int)lineX, 0,
			(int)lineX, 720,
			RED
		);

		//文字列
		Novice::ScreenPrintf(20, 20, "AD: Move Player");
		Novice::ScreenPrintf(20, 60, "Arrow Keys: Move Scroll Start Line");
		Novice::ScreenPrintf(20, 140, "Scroll X (on World Axis): %.f", camera.pos.x);
		Novice::ScreenPrintf(20, 180, "Scroll Start Line(on Screen Axis): %.f", lineX);
		Novice::ScreenPrintf(20, 220, "player.pos.x: %.f", player.pos.x);

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
