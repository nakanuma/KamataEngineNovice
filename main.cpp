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
	int circleGH[2][2] = {
		{Novice::LoadTexture("./Resources/images/Circle.png"),
		Novice::LoadTexture("./Resources/images/CircleHit.png"),},
		{Novice::LoadTexture("./Resources/images/CircleW.png"),
		Novice::LoadTexture("./Resources/images/CircleWHit.png"),}
	};

	struct Vector2 { 
		float x; 
		float y; 
	};

	struct Player {
		Vector2 pos;
		float width;
		float height;
		float speed;
	};

	//プレイヤーの情報
	Player player{
		{320.0f,360.0f },
		32.0f,
		128.0f,
		4.0f,
	};

	//画像の切り替えに使用するフラグ
	bool isBackgroundVisible = true;
	bool isChangeColor = false;

	//当たり判定に使用するフラグ
	bool isHitX = false;
	bool isHitY = false;
	bool isHit = false;

	bool isHitA = false;
	bool isHitB = false;
	bool isHitC = false;
	bool isHitD = false;

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

		//プレイヤーの移動処理
		if (keys[DIK_W]) {
			player.pos.y -= player.speed;
		}

		if (keys[DIK_S]) {
			player.pos.y += player.speed;
		}

		if (keys[DIK_A]) {
			player.pos.x -= player.speed;
		}

		if (keys[DIK_D]) {
			player.pos.x += player.speed;
		}

		//プレイヤーの拡縮
		if (keys[DIK_Q]) {
			player.width += 4;
		}

		if (keys[DIK_E]) {
			player.width -= 4;
		}

		if (keys[DIK_Z]) {
			player.height += 4;
		}

		if (keys[DIK_C]) {
			player.height -= 4;
		}

		//拡縮の限界値を設定
		if (player.width > 256.0f) {
			player.width = 256.0f;
		}

		if (player.width < 4.0f) {
			player.width = 4.0f;
		}

		if (player.height > 256.0f) {
			player.height = 256.0f;
		}

		if (player.height < 4.0f) {
			player.height = 4.0f;
		}

		//X方向の当たり判定
		isHitA = player.pos.x - player.width / 2 < circleRightX;
		isHitB = player.pos.x + player.width / 2 > circleLeftX;
		isHitX = isHitA && isHitB;

		//Y方向の当たり判定
		isHitC = player.pos.y - player.height / 2 < circleBottomY;
		isHitD = player.pos.y + player.height / 2 > circleTopY;
		isHitY = isHitC && isHitD;

		//XとYが同時に当たっている場合
		isHit = isHitX && isHitY;

		//同時に当たっている場合、色を変更
		isChangeColor = isHit;

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
			circleGH[isBackgroundVisible][isChangeColor],
			4.0f, 4.0f,
			0.0f,
			0xFFFFFFFF
		);

		//プレイヤーの描画
		Novice::DrawQuad(
			(int)player.pos.x - (int)player.width / 2, (int)player.pos.y + (int)player.height / 2,
			(int)player.pos.x + (int)player.width / 2, (int)player.pos.y + (int)player.height / 2,
			(int)player.pos.x - (int)player.width / 2, (int)player.pos.y - (int)player.height / 2,
			(int)player.pos.x + (int)player.width / 2, (int)player.pos.y - (int)player.height / 2,
			0, 0,
			32, 32,
			circleGH[isBackgroundVisible][isChangeColor],
			0xFFFFFFFF
		);

		//操作方法を表す文字列
		Novice::ScreenPrintf(30, 30,"WASD : Move");
		Novice::ScreenPrintf(30, 60, "QE   : Change width");
		Novice::ScreenPrintf(30, 90, "ZC   : Change height");
		Novice::ScreenPrintf(30, 120, "Space: Change background color");

		//当たり判定を表す文字列
		if (isHitX) {
			Novice::ScreenPrintf(670, 30, "isHitX: TRUE");
		} else {
			Novice::ScreenPrintf(670, 30, "isHitX: FALSE");
		}

		if (isHitA) {
			Novice::ScreenPrintf(700, 60, "if(PlayerLeftX( %.f) < TargetRightX( %.f) : TRUE", player.pos.x - player.width / 2, circleRightX);
		} else {
			Novice::ScreenPrintf(700, 60, "if(PlayerLeftX( %.f) < TargetRightX( %.f) : FALSE", player.pos.x - player.width / 2, circleRightX);
		}

		if (isHitB) {
			Novice::ScreenPrintf(700, 90, "if(TargetLeftX( %.f) < PlayerRightX( %.f) : TRUE", circleLeftX, player.pos.x + player.width / 2);
		} else {
			Novice::ScreenPrintf(700, 90, "if(TargetLeftX( %.f) < PlayerRightX( %.f) : FALSE", circleLeftX, player.pos.x + player.width / 2);
		}

		if (isHitY) {
			Novice::ScreenPrintf(670, 120, "isHitY: TRUE");
		} else {
			Novice::ScreenPrintf(670, 120, "isHitY: FALSE");
		}

		if (isHitC) {
			Novice::ScreenPrintf(700, 150, "if(PlayerTopY( %.f) < TargetBottomY( %.f) : TRUE", player.pos.y - player.width / 2, circleBottomY);
		} else {
			Novice::ScreenPrintf(700, 150, "if(PlayerTopY( %.f) < TargetBottomY( %.f) : FALSE", player.pos.y - player.width / 2, circleBottomY);
		}

		if (isHitD) {
			Novice::ScreenPrintf(700, 180, "if(TargetTopY( %.f) < PlayerBottomY( %.f) : TRUE", circleTopY, player.pos.y + player.width / 2);
		} else {
			Novice::ScreenPrintf(700, 180, "if(TargetTopY( %.f) < PlayerBottomY( %.f) : FALSE", circleTopY, player.pos.y + player.width / 2);
		}

		if (isHit) {
			Novice::ScreenPrintf(670, 210, "isHit: TRUE");
		} else {
			Novice::ScreenPrintf(670, 210, "isHit: FALSE");
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
