#include <Novice.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_AL1_13_確認課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//画像読み込み
	int circleGH[] = {
		Novice::LoadTexture("./Resources/images/Circle.png"),
		Novice::LoadTexture("./Resources/images/CircleW.png"),
	};

	int circleHitGH[] = {
		Novice::LoadTexture("./Resources/images/CircleHit.png"),
		Novice::LoadTexture("./Resources/images/CircleWHit.png"),
	};

	//画像の切り替えに使用するフラグ
	bool isBackgroundVisible = true;

	//中央の円の情報
	float circleCenterX = 640.0f; //中央X座標
	float circleCenterY = 360.0f; //中央Y座標

	float circleW = 128.0f; //横幅
	float circleH = 128.0f; //縦幅

	float circleRightX = circleCenterX + circleW / 2;  //右側のX座標
	float circleLeftX = circleCenterX - circleW / 2;   //左側のX座標
													   
	float circleBottomY = circleCenterY + circleH / 2; //下側のY座標
	float circleTopY = circleCenterY - circleH / 2;    //上側のY座標

	//プレイヤーの情報


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

		//スペースキーを押した際に画像を切り替える処理
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			isBackgroundVisible = !isBackgroundVisible;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//線の描画
		Novice::DrawLine(640, 0, 640, 960, WHITE);  //縦
		Novice::DrawLine(0, 360, 1280, 360, WHITE); //横

		//中央の円の描画
		Novice::DrawSpriteRect(
			(int)circleCenterX - (int)circleW / 2, (int)circleCenterY - (int)circleH / 2,
			0, 0,
			32, 32,
			circleGH[isBackgroundVisible],
			4.0f, 4.0f,
			0.0f,
			0xFFFFFFFF
		);

		//プレイヤーを描画


		//操作方法を表す文字列
		Novice::ScreenPrintf(30, 30,"WASD : Move");
		Novice::ScreenPrintf(30, 60, "QE   : Change width");
		Novice::ScreenPrintf(30, 90, "ZC   : Change height");
		Novice::ScreenPrintf(30, 120, "Space: Change background color");

		//テスト用文字列
		Novice::ScreenPrintf(30, 500, "%.f", circleRightX);
		Novice::ScreenPrintf(30, 520, "%.f", circleLeftX);
		Novice::ScreenPrintf(30, 540, "%.f", circleBottomY);
		Novice::ScreenPrintf(30, 560, "%.f", circleTopY);

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
