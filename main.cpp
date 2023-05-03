#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//自機の初期位置
	int playerX = 640;
	int playerY = 360;

	//自機の移動速度
	int playerSpd = 5;

	//アニメーションタイマー
	int animationTimer = 0;

	//グローバルタイマー
	long long globalTimer = 0;

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
	enum State {
		BACK,
		FRONT,
		LEFT,
		RIGHT
	};

	State playerState = FRONT;

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
	
		//グローバルタイマーをインクリメント
		globalTimer++;

		//15フレーム毎
		if (globalTimer % 15 == 0) {
			//アニメーションタイマーをインクリメント
			animationTimer++;

			//4を超えたら0にする
			if (animationTimer >= 4) {
				animationTimer = 0;
			}
		}

		//状態遷移
		switch (playerState) {
		case BACK:
			if (keys[DIK_S]) {
				playerState = FRONT;
			}
			if (keys[DIK_A]) {
				playerState = LEFT;
			}
			if (keys[DIK_D]) {
				playerState = RIGHT;
			}
			break;

		case FRONT:
			if (keys[DIK_W]) {
				playerState = BACK;
			}
			if (keys[DIK_A]) {
				playerState = LEFT;
			}
			if (keys[DIK_D]) {
				playerState = RIGHT;
			}
			break;

		case LEFT:
			if (keys[DIK_W]) {
				playerState = BACK;
			}
			if (keys[DIK_S]) {
				playerState = FRONT;
			}
			if (keys[DIK_D]) {
				playerState = RIGHT;
			}
			break;

		case RIGHT:
			if (keys[DIK_W]) {
				playerState = BACK;
			}
			if (keys[DIK_A]) {
				playerState = LEFT;
			}
			if (keys[DIK_S]) {
				playerState = FRONT;
			}
			break;
		}

		//移動処理
		playerX += (Novice::CheckHitKey(DIK_D) - Novice::CheckHitKey(DIK_A)) * playerSpd;

		playerY += (Novice::CheckHitKey(DIK_S) - Novice::CheckHitKey(DIK_W)) * playerSpd;

		//画面端で停止
		playerX = min(max(playerX, 0), 1280 - 128);

		playerY = min(max(playerY, 0), 720 - 164);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//状態による描画
		int playerGH = -1;

		switch (playerState) {
		case BACK:
			playerGH = backGH[animationTimer];
			break;
		case FRONT:
			playerGH = frontGH[animationTimer];
			break;
		case LEFT:
			playerGH = leftGH[animationTimer];
			break;
		case RIGHT:
			playerGH = rightGH[animationTimer];
			break;
		}

		Novice::DrawSprite(playerX, playerY, playerGH, 1, 1, 0.0f, 0xFFFFFFFF);

		//タイマーを表示
		Novice::ScreenPrintf(0, 0, "globaltimer: %d", globalTimer);
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
