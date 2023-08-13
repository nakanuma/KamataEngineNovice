#include <Novice.h>
#include "Easing.h"

const char kWindowTitle[] = "演算兵士";

//マウスの位置を格納
int mouseX, mouseY;

//ゲームのシーン
enum Scene {
	TITLE,
	STAGESELECT,
	GAME,
	GAMECLEAR,
	GAMEOVER,
};

Scene scene = STAGESELECT;//初期値を代入。ここを変更でデバッグを行う

//ステージの選択
enum Stage {
	STAGE1,
	STAGE2,
	STAGE3,
};

Stage stage = STAGE1;//初期値を代入



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	///
	///画像読み込み
	///

	///
	///↓タイトルで使用する画像
	/// 

	int titleGH = Novice::LoadTexture("./Resources/images/title.png");//タイトル画面
	int gameStartButtonGH = Novice::LoadTexture("./Resources/images/gameStartButton.png");//ゲーム開始ボタン
	int gameEndButtonGH = Novice::LoadTexture("./Resources/images/gameEndButton.png");//終了ボタン

	///
	///↓ステージセレクトで使用する画像
	/// 

	int stageSelectGH = Novice::LoadTexture("./Resources/images/stageSelect.png");//ステージ選択画面
	int returnButtonGH = Novice::LoadTexture("./Resources/images/returnButton.png");//戻るボタン
	int stage1ButtonGH = Novice::LoadTexture("./Resources/images/stage1Button.png");//ステージ1のボタン
	int stage2ButtonGH = Novice::LoadTexture("./Resources/images/stage2Button.png");//ステージ2のボタン
	int stage3ButtonGH = Novice::LoadTexture("./Resources/images/stage3Button.png");//ステージ3のボタン
	//int starGH = Novice::LoadTexture("./Resources/images/star.png");//星
	int emptyStarGH = Novice::LoadTexture("./Resources/images/emptyStar.png");//空の星
	int stageSelectFlameGH = Novice::LoadTexture("./Resources/images/stageSelectFlame.png");//ステージを選択する赤枠
	int startButtonGH = Novice::LoadTexture("./Resources/images/startButton.png");//スタートボタン

	///
	/// ↓タイトルで使用する変数
	/// 

	//ゲーム開始ボタンと終了ボタンの色を格納
	int gameStartButtonColor,gameEndButtonColor;

	///
	/// ↓ステージセレクトで使用する変数
	/// 
	
	//戻るボタンの色を格納
	int returnButtonColor;
	//ステージ選択ボタンの色を格納
	int stage1ButtonColor, stage2ButtonColor, stage3ButtonColor;
	//スタートボタンの色を格納
	int startButtonColor;

	//シーン遷移で使用する海苔
	int transitionBoxPosX = 1280;//海苔のx座標。ステージセレクトに戻る際に初期化し直す必要がある
	bool isTransitionBox = false;//シーン遷移が行われているかのフラグ。ステージセレクトに戻る際に初期化し直す必要がある
	int sceneTransitionCount = 45;//シーンが切り替わるまでのカウント。ステージセレクトに戻る際に初期化し直す必要がある

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		//マウスの位置を取得
		Novice::GetMousePosition(&mouseX, &mouseY);

		switch (scene) {

			///
			/// ↓タイトルここから
			/// 
			
		case TITLE:
			///
			/// ↓更新処理ここから
			/// 
			
			//マウスがゲーム開始ボタンの上にある場合の処理
			if (mouseX > 400 && mouseX < 880 && mouseY>432 && mouseY < 528) {
				//押した場合、シーンをステージセレクトに遷移
				if (Novice::IsPressMouse(0)) {
					scene = STAGESELECT;
				}
				//色を濃くする
				gameStartButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				gameStartButtonColor = 0xFFFFFFFF;
			}

			//マウスが終了ボタンの上にある場合の処理
			if (mouseX > 400 && mouseX < 880 && mouseY>576 && mouseY < 672) {
				//押した場合、ゲームを終了
				if (Novice::IsPressMouse(0)) {
					return 0;
				}
				//色を濃くする
				gameEndButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				gameEndButtonColor = 0xFFFFFFFF;
			}

			///
			/// ↑更新処理ここまで
			/// 
			
			///
			/// ↓描画処理ここから
			/// 

			//タイトル背景を描画
			Novice::DrawSprite(
				0, 0,
				titleGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//ゲーム開始ボタンを描画
			Novice::DrawSprite(
				400, 432,
				gameStartButtonGH,
				1.0f, 1.0f,
				0.0f,
				gameStartButtonColor
			);

			//ゲーム終了ボタンを描画
			Novice::DrawSprite(
				400, 576,
				gameEndButtonGH,
				1.0f, 1.0f,
				0.0f,
				gameEndButtonColor
			);

			///
			/// ↑描画処理ここまで
			/// 
			
			break;

			///
			/// ↑タイトルここまで
			/// 

			///
			/// ↓ステージセレクトここから
			/// 

		case STAGESELECT:
			///
			/// ↓更新処理ここから
			/// 
			

			//マウスが戻るボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 12 && mouseX < 108 && mouseY>12 && mouseY < 108 && isTransitionBox==false) {
				//押した場合、タイトルに戻る
				if (Novice::IsPressMouse(0)) {
					scene = TITLE;
				}
				//色を濃くする
				returnButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				returnButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ1ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 158 && mouseX < 374 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ1にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE1;
				}
				//色を濃くする
				stage1ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage1ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ2ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 532 && mouseX < 748 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ2にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE2;
				}
				//色を濃くする
				stage2ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage2ButtonColor = 0xFFFFFFFF;
			}

			//マウスがステージ3ボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 906 && mouseX < 1122 && mouseY>156 && mouseY < 372 && isTransitionBox == false) {
				//押した場合、ステージ3にする
				if (Novice::IsPressMouse(0)) {
					stage = STAGE3;
				}
				//色を濃くする
				stage3ButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				stage3ButtonColor = 0xFFFFFFFF;
			}

			//マウスがスタートボタンの上にある場合の処理（シーン遷移時は反応しないようにする）
			if (mouseX > 822 && mouseX < 1206 && mouseY>552 && mouseY < 648 && isTransitionBox == false) {
				//押した場合
				if (Novice::IsPressMouse(0)) {
					//シーン遷移が行われる
					isTransitionBox = true;
					//選択したステージ毎に初期化を行う
					if (stage == STAGE1) {
						
					}
					if (stage == STAGE2) {
						
					}
					if (stage == STAGE3) {
						
					}
				}
				//色を濃くする
				startButtonColor = 0x808080FF;
			} else {
				//マウスがボタンの上に無い場合、色をそのままにする
				startButtonColor = 0xFFFFFFFF;
			}

			//シーン遷移カウント（45フレーム）が0になったらシーン遷移する
			if (sceneTransitionCount <= 0) {
				scene = GAME;
			}

			///
			/// ↑更新処理ここまで
			/// 

			///
			/// ↓描画処理ここから
			/// 

			//ステージセレクト背景を描画
			Novice::DrawSprite(
				0, 0,
				stageSelectGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//戻るボタンを描画
			Novice::DrawSprite(
				12, 12,
				returnButtonGH,
				1.0f, 1.0f,
				0.0f,
				returnButtonColor
			);

			//ステージ1ボタンを描画
			Novice::DrawSprite(
				158, 156,
				stage1ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage1ButtonColor
			);

			//ステージ2ボタンを描画
			Novice::DrawSprite(
				532, 156,
				stage2ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage2ButtonColor
			);

			//ステージ3ボタンを描画
			Novice::DrawSprite(
				906, 156,
				stage3ButtonGH,
				1.0f, 1.0f,
				0.0f,
				stage3ButtonColor
			);

			//選択したステージの赤枠を描画（ステージを選択する処理と連動）
			Novice::DrawSprite(
				158 + (stage * 374), 156,
				stageSelectFlameGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFFF
			);

			//空の星をステージボタンの下に描画
			for (int i = 0; i < 3; i++) {
				//ステージ1の空の星を描画
				Novice::DrawSprite(
					158 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//ステージ2の空の星を描画
				Novice::DrawSprite(
					532 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//ステージ3の空の星を描画
				Novice::DrawSprite(
					906 + (i * 72), 372,
					emptyStarGH,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
			}

			//スタートボタンを描画
			Novice::DrawSprite(
				822, 552,
				startButtonGH,
				1.0f, 1.0f,
				0.0f,
				startButtonColor
			);

			///
			/// ↑描画処理ここまで
			/// 

			break;

			///
			/// ↑ステージセレクトここまで
			/// 
			
			///
			/// ↓ゲームここから
			/// 
			case GAME:
				///
				/// ↓更新処理ここから
				/// 



				///
				/// ↑更新処理ここまで
				/// 

				///
				/// ↓描画処理ここから
				///
				


				///
				/// ↑描画処理ここまで
				///
				
				break;

			case GAMEOVER:
				break;
			case GAMECLEAR:
				break;
		}

		//ステージセレクト画面のシーン遷移で使用する海苔を描画
		if (isTransitionBox == true) {
			Novice::DrawBox(
				transitionBoxPosX, 0,
				1920, 720,
				0.0f,
				BLACK,
				kFillModeSolid
			);
			//海苔を移動させる
			if (transitionBoxPosX > -1920) {
				transitionBoxPosX -= 30;
			}
			if (sceneTransitionCount > 0) {
				sceneTransitionCount--;
			}
		}

		//デバッグ用

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
