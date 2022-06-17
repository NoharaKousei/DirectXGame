#include "GameScene.h"
#include "AxisIndicator.h"
#include "DebugCamera.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "math.h"
#include "random"
#include <DirectXMath.h>
#include <cassert>

/// <summary>
/// 単位行列
/// </summary>
/// <returns>単位行列</returns>
Matrix4 CreateMatIdentity() {
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1;
	matIdentity.m[1][1] = 1;
	matIdentity.m[2][2] = 1;
	matIdentity.m[3][3] = 1;

	return matIdentity;
}

/// <summary>
/// スケーリング行列の作成
/// </summary>
/// <param name="scale">ワールドトランスフォームのスケール</param>
/// <returns>スケーリング行列</returns>
Matrix4 CreateMatScale(Vector3 scale) {

	Matrix4 matScale = CreateMatIdentity();
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;

	return matScale;
}

/// <summary>
/// 平行移動行列の作成
/// </summary>
/// <param name="scale">ワールドトランスフォームのトランスレーション</param>
/// <returns>平行移動行列</returns>
Matrix4 CreateMatTrans(Vector3 translation) {

	Matrix4 matTrans = CreateMatIdentity();
	matTrans.m[0][0] = translation.x;
	matTrans.m[1][1] = translation.y;
	matTrans.m[2][2] = translation.z;

	return matTrans;
}

/// <summary>
/// X軸回転行列の作成
/// </summary>
/// <param name="rotation">ワールドトランスフォームのローテーション</param>
/// <returns>X軸回転行列</returns>
Matrix4 CreateMatRotX(Vector3 rotationX) {

	Matrix4 matRot = CreateMatIdentity();
	matRot.m[0][0] = rotationX.x;
	matRot.m[1][1] = rotationX.y;
	matRot.m[2][2] = rotationX.z;

	return matRot;
}

/// <summary>
/// Y軸回転行列の作成
/// </summary>
/// <param name="rotationY">ワールドトランスフォームのローテーション</param>
/// <returns>Y軸回転行列</returns>
Matrix4 CreateMatRotY(Vector3 rotationY) {

	Matrix4 matRot = CreateMatIdentity();
	matRot.m[0][0] = rotationY.x;
	matRot.m[1][1] = rotationY.y;
	matRot.m[2][2] = rotationY.z;

	return matRot;
}

/// <summary>
/// Z軸回転行列の作成
/// </summary>
/// <param name="rotationY">ワールドトランスフォームのローテーション</param>
/// <returns>Z軸回転行列</returns>
Matrix4 CreateMatRotZ(Vector3 rotationZ) {

	Matrix4 matRot = CreateMatIdentity();
	matRot.m[0][0] = rotationZ.x;
	matRot.m[1][1] = rotationZ.y;
	matRot.m[2][2] = rotationZ.z;

	return matRot;
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	//デストラクタ
	delete model_;

	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg"); //ファイル名を指定して読み込む

	// 3Dモデルの生成
	model_ = Model::Create();

	//乱数シード
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float> dist(-0.5f, 5.0f);

	//乱数エンジンを渡し指定範囲からランダムな数値を得る
	float value = dist(engine);

	//回転角用乱数範囲を設定
	std::uniform_real_distribution<float> rotDist(0.0f, DirectX::XM_2PI);

	//座標用の乱数範囲を設定
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//範囲FORで全てのワールドトランスフォームを順に処理する
	for (int i = 0; i < _countof(worldTransforms_); i++) {
		//ワールドトランスフォームの初期化
		worldTransforms_[i].Initialize();

		////デバッグカメラの生成
		// debugCamera_ = new DebugCamera(1200, 800);

		////軸方向表示を有効にする
		// AxisIndicator::GetInstance()->SetVisible(true);

		////軸方向表示が参照するビュープロジェクションを指定する
		// AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

		////ライン描画が参照するビュープロジェクションを指定する
		// PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

		// XYZ軸方向のスケーリングを設定
		worldTransforms_[i].scale_ = { 1, 1, 1 };
		// XYZ周りの平行移動を設定
		worldTransforms_[i].translation_ = { posDist(engine), posDist(engine), posDist(engine) };
		// Z周りの回転角を設定
		worldTransforms_[i].rotation_ = { rotDist(engine), rotDist(engine), rotDist(engine) };

		Matrix4 matIdentity;
		matIdentity.m[0][0] = 1;
		matIdentity.m[1][1] = 1;
		matIdentity.m[2][2] = 1;
		matIdentity.m[3][3] = 1;

		//スケーリング行列を宣言
		Matrix4 matScale = CreateMatScale(worldTransforms_[i].scale_);

		// Z軸回転行列を設定
		Matrix4 matRotZ;

		// X軸回転行列を設定
		Matrix4 matRotX;

		// Y軸回転行列を設定
		Matrix4 matRotY;

		//平行移動行列を設定
		Matrix4 matTrans = MathUtility::Matrix4Identity();

		//スケーリング倍率を行列に設定する
		matScale.m[0][0] = worldTransforms_[i].scale_.x;
		matScale.m[1][1] = worldTransforms_[i].scale_.y;
		matScale.m[2][2] = worldTransforms_[i].scale_.z;
		matScale.m[3][3] = 1;

		// Z軸回転行列の各要素を設定する
		matRotZ.m[0][0] = cos(worldTransforms_[i].rotation_.z);
		matRotZ.m[0][1] = -sin(worldTransforms_[i].rotation_.z);
		matRotZ.m[1][0] = sin(worldTransforms_[i].rotation_.z);
		matRotZ.m[1][1] = cos(worldTransforms_[i].rotation_.z);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Y軸回転行列の各要素を設定する
		matRotY.m[0][0] = cos(worldTransforms_[i].rotation_.y);
		matRotY.m[0][2] = -sin(worldTransforms_[i].rotation_.y);
		matRotY.m[2][0] = sin(worldTransforms_[i].rotation_.y);
		matRotY.m[2][2] = cos(worldTransforms_[i].rotation_.y);
		matRotY.m[1][1] = 1;
		matRotY.m[3][3] = 1;

		// X軸回転行列の各要素を設定する
		matRotX.m[1][1] = cos(worldTransforms_[i].rotation_.x);
		matRotX.m[1][2] = sin(worldTransforms_[i].rotation_.x);
		matRotX.m[2][1] = -sin(worldTransforms_[i].rotation_.x);
		matRotX.m[2][2] = cos(worldTransforms_[i].rotation_.x);
		matRotX.m[0][0] = 1;
		matRotX.m[3][3] = 1;

		//合成用回転行列を設定
		Matrix4 matRot = MathUtility::Matrix4Identity();

		//各軸の回転行列を合成
		matRot *= matRotZ;
		matRot *= matRotX;
		matRot *= matRotY;

		//移動量を行列に設定する
		matTrans.m[3][0] = worldTransforms_[i].translation_.x;
		matTrans.m[3][1] = worldTransforms_[i].translation_.y;
		matTrans.m[3][2] = worldTransforms_[i].translation_.z;

		worldTransforms_[i].matWorld_ = matIdentity;
		worldTransforms_[i].matWorld_ *= matScale;
		worldTransforms_[i].matWorld_ *= matRot;
		worldTransforms_[i].matWorld_ *= matTrans;
		//行列の転送
		worldTransforms_[i].TransferMatrix();
	}

	//カメラ視点座標を固定
	viewProjection_.eye = { 0, 0, -10 };

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1200, 800);

	//軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	//軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	//ライン描画が参照するビュープロジェクションを指定する
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {

	debugCamera_->Update();

	//視点の移動ベクトル
	Vector3 move = { 0, 0, 0 };

	//視点の移動の速さ
	const float kEyeSpeed = 0.2f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_W)) {
		move.z += kEyeSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.z -= kEyeSpeed;
	}
	//視点移動
	viewProjection_.eye += move;
	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバック用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	//注視点移動処理
	{
		//注視点の移動ベクトル
		Vector3 move = { 0, 0, 0 };
		//注視点の移動の速さ
		const float kTargetSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move.x -= kTargetSpeed;
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move.x = kTargetSpeed;
		}

		//視点移動（ベクトルの加算）
		viewProjection_.eye += move;
		//
		//デバック用表示
		debugText_->SetPos(50, 70);
		debugText_->Printf(
			"targe:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
			viewProjection_.target.z);
	}

	//上方向回転処理
	{
		//上方向の回転速さ[ラジアン/frame]
		const float kUpRotSpeed = 0.05f;
		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_SPACE)) {
			viewAngle += kUpRotSpeed;
			// 2πを超えたら0に戻す
			viewAngle = fmodf(viewAngle, 3.14 / 4 * 2.0f);
		}
		//上方向ベクトルを計算(半径1の円周上の座標)
		viewProjection_.up = { cosf(viewAngle), sinf(viewAngle), 0.0f };

		//行列の再計算
		viewProjection_.UpdateMatrix();
		//デバック用表示
		debugText_->SetPos(50, 90);
		debugText_->Printf(
			"up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/// 3Dモデル描画
	for (int i = 0; i < _countof(worldTransforms_); i++) {
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}
	///ライン描画が参照するビュープロジェクションを指定する
	PrimitiveDrawer::GetInstance()->DrawLine3d({ 0, 0, 0 }, { 0, 0, 10 }, { 0, 0, 1, 1 });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
