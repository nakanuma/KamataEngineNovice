#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_MT1_14_00_確認課題";

//構造体
struct Vector2 {
	float x;
	float y;
};

//点の構造体
struct Point {
	Vector2 start;
	Vector2 end;
};

//円の構造体
struct Circle {
	Vector2 pos;
	float radius;
	unsigned int color;
};

//座標変換を行う関数
Vector2 WorldToScreen(Vector2 world) {
	Vector2 screen{
		world.x,
		500 - world.y
	};
	return screen;
}

//内積を計算する関数
float dot(Vector2 v1,Vector2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

//ベクトルの長さを求める関数
float vector_length(Vector2 v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

//ベクトルを正規化する関数
Vector2 normalize(Vector2 v) {
	float length = vector_length(v);
	Vector2 result = { v.x / length,v.y / length };
	return result;
}

//値を0から1の範囲に収める関数
float clamp(float value) {
	if (value < 0.0) {
		return 0.0;
	} else if(value>1.0){
		return 1.0;
	} else {
		return value;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//点の情報
	Point point{
		100.0f,100.0f,
		1000.0f,400.0f,
	};

	//プレイヤーの情報
	Circle player{
		500.0f,0.0f,
		30.0f,
		WHITE
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
		if (keys[DIK_UP]) {
			player.pos.y += 8.0f;
		}

		if (keys[DIK_DOWN]) {
			player.pos.y -= 8.0f;
		}

		if (keys[DIK_LEFT]) {
			player.pos.x -= 8.0f;
		}

		if (keys[DIK_RIGHT]) {
			player.pos.x += 8.0f;
		}

		//カプセルと円の衝突判定

		//線分の始点から円の中心までのベクトルを計算
		Vector2 d = { player.pos.x - point.start.x, player.pos.y - point.start.y };

		//線分の方向ベクトルを計算
		Vector2 ba = { point.end.x - point.start.x,point.end.y - point.start.y };

		//baを正規化
		Vector2 e = normalize(ba);

		//tの値を求める
		float t = dot(d, e) / vector_length(ba);

		//tを0~1の範囲に収める
		t = clamp(t);

		//線形補間
		Vector2 f = { (1.0f - t) * point.start.x + t * point.end.x ,(1.0f - t) * point.start.y + t * point.end.y };

		Vector2 m = { player.pos.x - f.x,player.pos.y - f.y };

		//距離を求める
		float distance = vector_length(m);

		//衝突している場合、色を変更
		if (distance < player.radius + 60.0f) {
			player.color = RED;
		} else {
			player.color = WHITE;
		}

		//点の座標をスクリーンに変換
		Vector2 pointStartScreen = WorldToScreen(point.start);
		Vector2 pointEndScreen = WorldToScreen(point.end);

		//プレイヤーの座標をスクリーンに変換
		Vector2 playerScreen = WorldToScreen(player.pos);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//点を繋いで線を描画
		Novice::DrawLine(
			(int)pointStartScreen.x, (int)pointStartScreen.y,
			(int)pointEndScreen.x, (int)pointEndScreen.y,
			WHITE
		);

		//線の両端に円を描画
		Novice::DrawEllipse(
			(int)pointStartScreen.x, (int)pointStartScreen.y,
			60, 60,
			0.0f,
			WHITE,
			kFillModeWireFrame
		);

		Novice::DrawEllipse(
			(int)pointEndScreen.x, (int)pointEndScreen.y,
			60, 60,
			0.0f,
			WHITE,
			kFillModeWireFrame
		);

		//プレイヤーを描画
		Novice::DrawEllipse(
			(int)playerScreen.x, (int)playerScreen.y,
			(int)player.radius, (int)player.radius,
			0.0f,
			player.color,
			kFillModeSolid
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
