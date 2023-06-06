#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_AL1_07_確認課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//自機の情報
	float playerX = 640.0f;   //X座標
	float playerY = 360.0f;   //Y座標
	float playerSpd = 5.0f;   //速度

	//アニメーションタイマー
	int animationTimer = 0;

	//画像読み込み
	int backGH[] = {
		Novice::LoadTexture("./images/back0.png"),
		Novice::LoadTexture("./images/back1.png"),
		Novice::LoadTexture("./images/back2.png"),
		Novice::LoadTexture("./images/back3.png"),
	};
	int frontGH[] = {
		Novice::LoadTexture("./images/front0.png"),
		Novice::LoadTexture("./images/front1.png"),
		Novice::LoadTexture("./images/front2.png"),
		Novice::LoadTexture("./images/front3.png"),
	};
	int leftGH[] = {
		Novice::LoadTexture("./images/left0.png"),
		Novice::LoadTexture("./images/left1.png"),
		Novice::LoadTexture("./images/left2.png"),
		Novice::LoadTexture("./images/left3.png"),
	};
	int rightGH[] = {
		Novice::LoadTexture("./images/right0.png"),
		Novice::LoadTexture("./images/right1.png"),
		Novice::LoadTexture("./images/right2.png"),
		Novice::LoadTexture("./images/right3.png"),
	};

	//状態を管理する
	enum Direction {
		BACK,
		FRONT,
		LEFT,
		RIGHT
	};

	Direction playerDirection = FRONT;

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

		//状態遷移
		switch (playerDirection) {
		case BACK:
			if (keys[DIK_S]) {
				playerDirection = FRONT;
			}

			if (keys[DIK_A]) {
				playerDirection = LEFT;
			}

			if (keys[DIK_D]) {
				playerDirection = RIGHT;
			}

			break;

		case FRONT:
			if (keys[DIK_W]) {
				playerDirection = BACK;
			}

			if (keys[DIK_A]) {
				playerDirection = LEFT;
			}

			if (keys[DIK_D]) {
				playerDirection = RIGHT;
			}

			break;

		case LEFT:
			if (keys[DIK_W]) {
				playerDirection = BACK;
			}

			if (keys[DIK_S]) {
				playerDirection = FRONT;
			}

			if (keys[DIK_D]) {
				playerDirection = RIGHT;
			}

			break;

		case RIGHT:
			if (keys[DIK_W]) {
				playerDirection = BACK;
			}

			if (keys[DIK_A]) {
				playerDirection = LEFT;
			}

			if (keys[DIK_S]) {
				playerDirection = FRONT;
			}

			break;
		}

		//自機のキー移動
		if (Novice::CheckHitKey(DIK_W)) {
			playerY -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_A)) {
			playerX -= playerSpd;
		}

		if (Novice::CheckHitKey(DIK_S)) {
			playerY += playerSpd;
		}

		if (Novice::CheckHitKey(DIK_D)) {
			playerX += playerSpd;
		}

		//画面端で停止
		if (playerX < 0) {
			playerX = 0;
		}

		if (playerX > 1280 - 128) {
			playerX = 1280 - 128;
		}

		if (playerY < 0) {
			playerY = 0;
		}

		if (playerY > 720 - 164) {
			playerY = 720 - 164;
		}

		//アニメーションタイマーをインクリメント
		animationTimer++;

		//アニメーションタイマーが60を超えたら0に戻す
		if (animationTimer >= 60) {
			animationTimer = 0;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//状態による描画
		int playerGH = -1;

		switch (playerDirection) {
		case BACK:
			playerGH = backGH[animationTimer / 15];
			break;
		case FRONT:
			playerGH = frontGH[animationTimer / 15];
			break;
		case LEFT:
			playerGH = leftGH[animationTimer / 15];
			break;
		case RIGHT:
			playerGH = rightGH[animationTimer / 15];
			break;
		}

		//プレイヤーの描画
		Novice::DrawSprite(
			static_cast<int>(playerX),
			static_cast<int>(playerY),
			playerGH,
			1, 1,
			0.0f,
			0xFFFFFFFF);

		//タイマーを表示
		Novice::ScreenPrintf(0, 20, "animationTimer: %d", animationTimer);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();
		Sleep(16);

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
