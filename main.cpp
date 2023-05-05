#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//自機の半径
	int playerR = 16;

	//自機の速度
	int playerSpd = 8;

	//自機の初期位置
	int playerX = 640;
	int playerY = 360;

	//弾の最大数
	const int bulletNum = 8;

	//弾の初期位置と半径
	int bulletX[bulletNum];
	int bulletY[bulletNum];
	int bulletR = 8;

	//弾の速度
	int bulletSpd = 8;

	//弾のフラグ
	bool isBulletShot[bulletNum] = { false };

	//画像読み込み
	int player = Novice::LoadTexture("./images/player.png");
	int bullet = Novice::LoadTexture("./images/bullet.png");

	//弾のカウント
	int i;

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

		//自機のキー移動
		playerX += (Novice::CheckHitKey(DIK_D) - Novice::CheckHitKey(DIK_A)) * playerSpd;

		playerY += (Novice::CheckHitKey(DIK_S) - Novice::CheckHitKey(DIK_W)) * playerSpd;

		//画面外に出ないようにする
		playerX = min(max(playerX, playerR), 1280 - playerR);

		playerY = min(max(playerY, playerR), 720 - playerR);

		//スペースキーが押されたら弾を発射
		if (Novice::CheckHitKey(DIK_SPACE)) {
			for (i = 0; i <= bulletNum; i++) {
				if (!isBulletShot[i]) {
					isBulletShot[i] = true;
					bulletX[i] = playerX;
					bulletY[i] = playerY;
					break;
				}
			}
		}

		//弾の発射中、弾を上方向へ移動させて画面外に出たら弾フラグをfalseに
		for (i = 0; i <= bulletNum; i++) {
			if (isBulletShot[i]) {
				bulletY[i] -= bulletSpd;
				if (bulletY[i] < 0) {
					bulletY[i] = -200;
					isBulletShot[i] = false;
				}
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
	
		//弾を発射中、弾を描画
		for (i = 0; i <= bulletNum; i++) {
			Novice::DrawSprite(bulletX[i] - bulletR, bulletY[i] - bulletR, bullet, 1, 1, 0.0f, 0xFFFFFFFF);
		}

		//自機の描画
		Novice::DrawSprite(playerX - playerR, playerY - playerR, player, 1, 1, 0.0f, 0xFFFFFFFF);

		//キー操作を表す文字列
		Novice::ScreenPrintf(0, 0, "W:up A:left S:down D:right");
		Novice::ScreenPrintf(0, 20, "Space:shot");

		//自機の座標を表示
		Novice::ScreenPrintf(0, 60, "PlayerPosX:%5d      PlayerPosY:%5d", playerX, playerY);

		//弾のクールタイムを表示
		Novice::ScreenPrintf(0, 80, "bulletCoolTime: unko");

		//弾のフラグを表示
		for (i = 0; i < 8; i++) {
			Novice::ScreenPrintf(0, 100 + i * 20, "isBulletShot[%d]:%2d", i, isBulletShot[i]);
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
