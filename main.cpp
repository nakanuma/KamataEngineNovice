#include <Novice.h>

#define SOLDIER_NUM 10 //召喚する兵士の最大数
#define ENEMY_NUM 5 //出現する敵の最大数

const char kWindowTitle[] = "LC1B_24_ナカヌマカツシ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//画像読み込み
	int groundGH = Novice::LoadTexture("./Resources/images/ground.png"); //背景の地面
	int skyGH = Novice::LoadTexture("./Resources/images/sky.png"); //背景の空
	int towerGH = Novice::LoadTexture("./Resources/images/tower.png"); //タワー
	int soldierFlameGH = Novice::LoadTexture("./Resources/images/soldierFlame.png"); //兵士枠（数字、演算子、兵士の状態を表すための枠）
	int selectFlameGH = Novice::LoadTexture("./Resources/images/selectFlame.png"); //数字と演算子の選択中を示す枠
	int numberGH[10] = {
		Novice::LoadTexture("./Resources/images/0.png"),
		Novice::LoadTexture("./Resources/images/1.png"),
		Novice::LoadTexture("./Resources/images/2.png"),
		Novice::LoadTexture("./Resources/images/3.png"),
		Novice::LoadTexture("./Resources/images/4.png"),
		Novice::LoadTexture("./Resources/images/5.png"),
		Novice::LoadTexture("./Resources/images/6.png"),
		Novice::LoadTexture("./Resources/images/7.png"),
		Novice::LoadTexture("./Resources/images/8.png"),
		Novice::LoadTexture("./Resources/images/9.png"),
	}; //数字（24*24）
	int operatorGH[4] = {
		Novice::LoadTexture("./Resources/images/addition.png"),
		Novice::LoadTexture("./Resources/images/subtraction.png"),
		Novice::LoadTexture("./Resources/images/multiplication.png"),
		Novice::LoadTexture("./Resources/images/division.png"),
	}; //演算子（24*24）
	int number48xGH[5] = {
		Novice::LoadTexture("./Resources/images/48x0.png"),
		Novice::LoadTexture("./Resources/images/48x1.png"),
		Novice::LoadTexture("./Resources/images/48x2.png"),
		Novice::LoadTexture("./Resources/images/48x3.png"),
		Novice::LoadTexture("./Resources/images/48x4.png"),
	}; //数字（48*48）
	int operator48xGH[4] = {
		Novice::LoadTexture("./Resources/images/48xAddition.png"),
		Novice::LoadTexture("./Resources/images/48xSubtraction.png"),
		Novice::LoadTexture("./Resources/images/48xMultiplication.png"),
		Novice::LoadTexture("./Resources/images/48xDivision.png"),
	}; //演算子（48*48）
	int arrowKeysGH[4] = {
		Novice::LoadTexture("./Resources/images/down.png"),
		Novice::LoadTexture("./Resources/images/up.png"),
		Novice::LoadTexture("./Resources/images/left.png"),
		Novice::LoadTexture("./Resources/images/right.png"),
	}; //矢印キー
	int spaceGH = Novice::LoadTexture("./Resources/images/space.png"); //スペースキー
	int cooldownGH = Novice::LoadTexture("./Resources/images/cooldown.png"); //スペースキーのクールダウン表示用

	//位置を表す構造体
	struct Vector2 {
		float x;
		float y;
	};

	//兵士の構造体
	struct Soldier {
		Vector2 pos; //位置
		int num; //数字
		int ope; //演算子
		bool isAlive; //生存フラグ
	};
	float soldierSpeed = 2.0f; //速さ

	struct Soldier soldier[SOLDIER_NUM]; //兵士の配列を作成

	//兵士の情報を初期化
	for (int i = 0; i < SOLDIER_NUM; i++) {
		soldier[i].pos.x = 1124.0f;
		soldier[i].pos.y = 288.0f;
		soldier[i].num = 0;
		soldier[i].ope = 0;
		soldier[i].isAlive = false;
	}

	int soldierSummonCount = 0; //兵士を何体出したかのカウント

	//敵の構造体
	struct Enemy {
		Vector2 pos; //位置
		float speed; //速さ
		int num; //数字
		int numTemp; //現在の数字を格納
		int numDigit[3]; //表示用の数字を格納
		int targetNum; //目標の数字（この数字になったら消滅）
		int targetNumTemp; //目標の数字を格納
		int targetNumDigit[3]; //表示用の目標の数字を格納
		int summonFlame; //このフレームになったら出現
		bool isAlive; //生存フラグ
	};

	struct Enemy enemy[ENEMY_NUM]; //敵の配列を作成

	//敵の情報初期化用のデータ（手動で入力）
	float speeds[] = {
		1.0f,1.0f,1.0f,1.0f,1.0f, //1~5体目
	};
	int nums[] = {
		2,3,5,6,20, //1~5体目
	};
	int targetNums[] = {
		4,12,20,3,10, //1~5体目
	};
	int summonFlames[] = {
		180,360,540,720,900, //1~5体目
	};

	//敵の情報を初期化
	for (int i = 0; i < ENEMY_NUM; i++) {
		enemy[i].pos.x = 60.0f;
		enemy[i].pos.y = 288.0f;
		enemy[i].speed = speeds[i];
		enemy[i].num = nums[i];
		enemy[i].numTemp = 0;
		enemy[i].numDigit[0] = 0;
		enemy[i].numDigit[1] = 0;
		enemy[i].numDigit[2] = 0;
		enemy[i].targetNumTemp = 0;
		enemy[i].targetNumDigit[0] = 0;
		enemy[i].targetNumDigit[1] = 0;
		enemy[i].targetNumDigit[2] = 0;
		enemy[i].targetNum = targetNums[i];
		enemy[i].summonFlame = summonFlames[i];
		enemy[i].isAlive = false;
	}

	int playerTowerHP = 3; //タワーの体力

	//プレイヤーに持たせる数字と演算子をenumで宣言
	enum Number {
		ONE,
		TWO,
		THREE,
		FOUR
	};

	enum Operator {
		ADDITION,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION
	};

	Number playerNumber = ONE; //初期値を代入
	Operator playerOperator = ADDITION; //初期値を代入

	int arrowKeysColor[4]; //矢印キーの色を格納

	//兵士の召喚クールダウン用変数
	int soldierCooldownFrames = 60;
	int soldierCurrentCooldown = 0;
	bool canSoldierSummon = true;

	int gameCount = 0; //ゲームの経過フレーム数をカウント

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

		//プレイヤーが数字を選択する処理（ゲーム中）
		switch (playerNumber) {
		case(ONE):
			if (keys[DIK_UP] && !preKeys[DIK_UP]) {
				playerNumber = TWO;
			}
			break;
		case(TWO):
			if (keys[DIK_UP] && !preKeys[DIK_UP]) {
				playerNumber = THREE;
			}
			if (keys[DIK_DOWN] && !preKeys[DIK_DOWN]) {
				playerNumber = ONE;
			}
			break;
		case(THREE):
			if (keys[DIK_UP] && !preKeys[DIK_UP]) {
				playerNumber = FOUR;
			}
			if (keys[DIK_DOWN] && !preKeys[DIK_DOWN]) {
				playerNumber = TWO;
			}
			break;
		case(FOUR):
			if (keys[DIK_DOWN] && !preKeys[DIK_DOWN]) {
				playerNumber = THREE;
			}
			break;
		}

		//プレイヤーが演算子を選択する処理（ゲーム中）
		switch (playerOperator) {
		case(ADDITION):
			if (keys[DIK_RIGHT] && !preKeys[DIK_RIGHT]) {
				playerOperator = SUBTRACTION;
			}
			break;
		case(SUBTRACTION):
			if (keys[DIK_RIGHT] && !preKeys[DIK_RIGHT]) {
				playerOperator = MULTIPLICATION;
			}
			if (keys[DIK_LEFT] && !preKeys[DIK_LEFT]) {
				playerOperator = ADDITION;
			}
			break;
		case(MULTIPLICATION):
			if (keys[DIK_RIGHT] && !preKeys[DIK_RIGHT]) {
				playerOperator = DIVISION;
			}
			if (keys[DIK_LEFT] && !preKeys[DIK_LEFT]) {
				playerOperator = SUBTRACTION;
			}
			break;
		case(DIVISION):
			if (keys[DIK_LEFT] && !preKeys[DIK_LEFT]) {
				playerOperator = MULTIPLICATION;
			}
			break;
		}

		//キーを押した際に色を暗くする処理
		if (keys[DIK_DOWN]) {
			arrowKeysColor[0] = 0x808080FF;
		} else {
			arrowKeysColor[0] = 0xFFFFFFFF;
		} //下キー
		if (keys[DIK_UP]) {
			arrowKeysColor[1] = 0x808080FF;
		} else {
			arrowKeysColor[1] = 0xFFFFFFFF;
		} //上キー
		if (keys[DIK_LEFT]) {
			arrowKeysColor[2] = 0x808080FF;
		} else {
			arrowKeysColor[2] = 0xFFFFFFFF;
		} //左キー
		if (keys[DIK_RIGHT]) {
			arrowKeysColor[3] = 0x808080FF;
		} else {
			arrowKeysColor[3] = 0xFFFFFFFF;
		} //右キー

		//プレイヤーが兵士を召喚する処理
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] && canSoldierSummon) {
			for (int i = 0; i < SOLDIER_NUM; i++) {
				if (soldier[i].isAlive == false) {
					soldier[i].isAlive = true;
					soldier[i].pos.x = 1124.0f;
					soldier[i].num = playerNumber;
					soldier[i].ope = playerOperator;
					soldierSummonCount++;
					break;
				}
			}
			//クールダウン（1秒）
			soldierCurrentCooldown = soldierCooldownFrames;
			canSoldierSummon = false;
		}

		//クールダウンのカウントダウン
		if (soldierCurrentCooldown > 0) {
			soldierCurrentCooldown--;
		} else {
			canSoldierSummon = true;
		}

		//兵士の移動処理
		for (int i = 0; i < SOLDIER_NUM; i++) {
			if (soldier[i].isAlive == true) {
				soldier[i].pos.x -= soldierSpeed;
			}
		}

		//兵士が敵のタワーに当たったら消滅する処理
		for (int i = 0; i < SOLDIER_NUM; i++) {
			if (soldier[i].isAlive == true && soldier[i].pos.x < 24 + 168) {
				soldier[i].isAlive = false;
			}
		}

		//敵が出現する処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			//出現時間になった敵
			if (gameCount == enemy[i].summonFlame) {
				if (enemy[i].isAlive == false) {
					enemy[i].isAlive = true;
				}
			}
		}

		//敵の移動処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true) {
				enemy[i].pos.x += enemy[i].speed;
			}
		}

		//敵がプレイヤーのタワーに当たったら消滅する処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true && enemy[i].pos.x > 1088 - 96) {
				playerTowerHP--;
				enemy[i].isAlive = false;
			}
		}

		//兵士と敵が衝突した際の処理
		for (int i = 0; i < SOLDIER_NUM; i++) {
			for (int j = 0; j < ENEMY_NUM; j++) {
				//兵士が敵に衝突しているかつ兵士と敵が生存している場合
				if (soldier[i].pos.x < enemy[j].pos.x + 96 && soldier[i].isAlive == true && enemy[j].isAlive == true) {
					//プレイヤーの演算子が加算のとき
					if (soldier[i].ope == ADDITION) {
						enemy[j].num += soldier[i].num + 1;
						//プレイヤーの演算子が減算のとき
					} else if (soldier[i].ope == SUBTRACTION) {
						enemy[j].num -= soldier[i].num + 1;
						//プレイヤーの演算子が乗算のとき
					} else if (soldier[i].ope == MULTIPLICATION) {
						enemy[j].num *= soldier[i].num + 1;
						//プレイヤーの演算子が除算のとき
					} else if (soldier[i].ope == DIVISION) {
						enemy[j].num /= soldier[i].num + 1;
					}
					soldier[i].isAlive = false;
					break;
				}
			}
		}

		//敵の数字の上限と下限を設定（エラー回避）
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true) {
				if (enemy[i].num < 0) {
					enemy[i].num = 0;
				}
				if (enemy[i].num > 999) {
					enemy[i].num = 999;
				}
			}
		}

		//敵の数字を描画するために配列に格納する（衝突した処理の下に置く）
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true) {
				enemy[i].numTemp = enemy[i].num;
				enemy[i].numDigit[0] = (enemy[i].numTemp % 10);
				enemy[i].numTemp /= 10;
				enemy[i].numDigit[1] = (enemy[i].numTemp % 10);
				enemy[i].numTemp /= 10;
				enemy[i].numDigit[2] = (enemy[i].numTemp % 10);
				enemy[i].numTemp /= 10;
			}
		}

		//敵の目標の数字を描画するために配列に格納する
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true) {
				enemy[i].targetNumTemp = enemy[i].targetNum;
				enemy[i].targetNumDigit[0] = (enemy[i].targetNumTemp % 10);
				enemy[i].targetNumTemp /= 10;
				enemy[i].targetNumDigit[1] = (enemy[i].targetNumTemp % 10);
				enemy[i].targetNumTemp /= 10;
				enemy[i].targetNumDigit[2] = (enemy[i].targetNumTemp % 10);
				enemy[i].targetNumTemp /= 10;
			}
		}

		//敵の現在の数字が目標の数字と同じになった場合に消滅する処理
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].num == enemy[i].targetNum) {
				enemy[i].isAlive = false;
			}
		}

		//プレイヤーのタワーHPが0未満にならないようにする（エラー回避）
		if (playerTowerHP < 0) {
			playerTowerHP = 0;
		}

		gameCount++; //1ループ毎にゲームの経過フレームを増加

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//背景の空を描画
		Novice::DrawSprite(
			0, 0,
			skyGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//背景の地面を描画
		Novice::DrawSprite(
			0, 384,
			groundGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//敵のタワーを描画
		Novice::DrawSprite(
			24, 72,
			towerGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//プレイヤーのタワーを描画
		Novice::DrawSprite(
			1088, 72,
			towerGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//プレイヤーのタワーのHPを描画
		Novice::DrawSprite(
			1136, 36,
			numberGH[playerTowerHP],
			1.0f, 1.0f,
			0.0f,
			0x000000FF
		);
		Novice::DrawSprite(
			1136 + 24, 36,
			operatorGH[3],
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);
		Novice::DrawSprite(
			1136 + 24 + 24, 36,
			numberGH[3],
			1.0f, 1.0f,
			0.0f,
			0x000000FF
		);

		//兵士枠を描画
		Novice::DrawSprite(
			520, 432,
			soldierFlameGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//矢印を描画
		for (int i = 0; i < 2; i++) {
			Novice::DrawSprite(
				760, 612 - (i * 216),
				arrowKeysGH[i],
				1.0f, 1.0f,
				0.0f,
				arrowKeysColor[i]
			);
		} //上下キー
		for (int i = 0; i < 2; i++) {
			Novice::DrawSprite(
				484 + (i * 216), 672,
				arrowKeysGH[i + 2],
				1.0f, 1.0f,
				0.0f,
				arrowKeysColor[i + 2]
			);
		} //左右キー

		//スペースキーを描画
		Novice::DrawSprite(
			800, 504,
			spaceGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//スペースキーのクールダウン表示
		for (int i = 0; i < soldierCurrentCooldown; i++) {
			Novice::DrawSprite(
				800 + (i * 4), 504,
				cooldownGH,
				1.0f, 1.0f,
				0.0f,
				0xFFFFFFC0
			);
		}

		//選択した数字の赤枠を描画（プレイヤーが数字を選択する処理と連動）
		Novice::DrawSprite(
			712 - 1, 576 + 1 - (playerNumber * 48),
			selectFlameGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//選択した演算子の赤枠を描画（プレイヤーが演算子を選択する処理と連動）
		Novice::DrawSprite(
			520 + 1 + (playerOperator * 48), 624 - 1,
			selectFlameGH,
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//兵士枠（真ん中の枠）に数字を描画（プレイヤーが数字を選択する処理と連動）
		Novice::DrawSprite(
			616, 504,
			number48xGH[playerNumber + 1],
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//兵士枠（真ん中の枠）に演算子を描画（プレイヤーが演算子を選択する処理と連動）
		Novice::DrawSprite(
			568, 504,
			operator48xGH[playerOperator],
			1.0f, 1.0f,
			0.0f,
			0xFFFFFFFF
		);

		//召喚された兵士を描画
		for (int i = 0; i < SOLDIER_NUM; i++) {
			if (soldier[i].isAlive == true) {
				//箱を描画
				Novice::DrawBox(
					(int)soldier[i].pos.x, (int)soldier[i].pos.y,
					96, 96,
					0.0f,
					WHITE,
					kFillModeSolid
				);
				//演算子を描画
				Novice::DrawSprite(
					(int)soldier[i].pos.x + 24, (int)soldier[i].pos.y + 36,
					operatorGH[soldier[i].ope],
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				//数字を描画
				Novice::DrawSprite(
					(int)soldier[i].pos.x + 24 + 24, (int)soldier[i].pos.y + 36,
					numberGH[soldier[i].num + 1],
					1.0f, 1.0f,
					0.0f,
					0x000000FF
				);
			}
		}

		//出現した敵を描画
		for (int i = 0; i < ENEMY_NUM; i++) {
			if (enemy[i].isAlive == true) {
				Novice::DrawBox(
					(int)enemy[i].pos.x, (int)enemy[i].pos.y,
					96, 96,
					0.0f,
					WHITE,
					kFillModeSolid
				);
				//数字を描画
				for (int j = 0; j < 3; j++) {
					Novice::DrawSprite(
						(int)enemy[i].pos.x + 12 + (j * 24), (int)enemy[i].pos.y + 36,
						numberGH[enemy[i].numDigit[2 - j]],
						1.0f, 1.0f,
						0.0f,
						0x000000FF
					);
					//目標の数字を描画
					Novice::DrawSprite(
						(int)enemy[i].pos.x + 12 + (j * 24), (int)enemy[i].pos.y - 36,
						numberGH[enemy[i].targetNumDigit[2 - j]],
						1.0f, 1.0f,
						0.0f,
						0xFF0000FF
					);
				}
			}
		}

		//デバッグ用文字列
		Novice::ScreenPrintf(20, 10, "gameCount:%d", gameCount);
		Novice::ScreenPrintf(20, 30, "cooldown:%d", soldierCurrentCooldown);
		Novice::ScreenPrintf(20, 50, "soldierSummonCount:%d", soldierSummonCount);

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
