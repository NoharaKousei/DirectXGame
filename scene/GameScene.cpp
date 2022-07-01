#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "math.h"
#include "random"
#include "DebugCamera.h"
#include <DirectXMath.h>

//単位行列
Matrix4 CreateMatIdentity()
{
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1;
	matIdentity.m[1][1] = 1;
	matIdentity.m[2][2] = 1;
	matIdentity.m[3][3] = 1;

	return matIdentity;
}
//scale関数
Matrix4 CreateMatScale(Vector3 scale)
{
	Matrix4 matScale = CreateMatIdentity();
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;

	return matScale;
}
//translation関数
Matrix4 CreateMatTrans(Vector3 translation)
{
	Matrix4 matTrans = CreateMatIdentity();
	matTrans.m[3][0] = translation.x;
	matTrans.m[3][1] = translation.y;
	matTrans.m[3][2] = translation.z;

	return matTrans;
}
//rotationX関数
Matrix4 CreateMatRotX(Vector3 rotationX)
{
	Matrix4 matRot = CreateMatIdentity();
	matRot.m[1][1] = cos(rotationX.x);
	matRot.m[1][2] = sin(rotationX.x);
	matRot.m[2][1] = -sin(rotationX.x);
	matRot.m[2][2] = cos(rotationX.x);

	return matRot;
}
//rotationY関数
Matrix4 CreateMatRotY(Vector3 rotationY)
{
	Matrix4 matRot = CreateMatIdentity();
	matRot.m[0][0] = cos(rotationY.y);
	matRot.m[0][2] = -sin(rotationY.y);
	matRot.m[2][0] = sin(rotationY.y);
	matRot.m[2][2] = cos(rotationY.y);

	return matRot;
}
//rotationZ関数
Matrix4 CreateMatRotZ(Vector3 rotationZ)
{
	Matrix4 matRot = CreateMatIdentity();
	matRot.m[0][0] = cos(rotationZ.z);
	matRot.m[0][1] = sin(rotationZ.z);
	matRot.m[1][0] = -sin(rotationZ.z);
	matRot.m[1][1] = cos(rotationZ.z);

	return matRot;
}
//rotation合わさった関数
Matrix4 CreateMatRot(Vector3 rotation)
{
	Matrix4 matRot = CreateMatIdentity();
	matRot *= CreateMatRotZ(rotation);
	matRot *= CreateMatRotX(rotation);
	matRot *= CreateMatRotY(rotation);

	return matRot;

}

Matrix4 CreateMatWorld(Vector3 scale, Vector3 rotation, Vector3 trans)
{
	Matrix4 matWorld = CreateMatIdentity();
	matWorld *= CreateMatScale(scale);
	matWorld *= CreateMatRot(rotation);
	matWorld *= CreateMatTrans(trans);

	return matWorld;
}

Matrix4 CreateMatWorld(WorldTransform worldTransform) {
	Matrix4 matWorld = CreateMatIdentity();

	matWorld *= CreateMatScale(worldTransform.scale_);
	matWorld *= CreateMatTrans(worldTransform.translation_);
	matWorld *= CreateMatRot(worldTransform.rotation_);

	return matWorld;
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

	textureHandle_ = TextureManager::Load("mario.jpg");



	//3Dモデルの生成
	model_ = Model::Create();

	//乱数シード
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float>dist(-0.5f, 5.0f);

	//乱数エンジンを渡し指定範囲からランダムな数値を得る
	float value = dist(engine);

	//回転角用乱数範囲を設定
	std::uniform_real_distribution<float>rotDist(0.0f, DirectX::XM_2PI);
	//座標用の乱数範囲を設定
	std::uniform_real_distribution<float>posDist(-10.0f, 10.0f);

	//範囲FORで全てのワールドトランスフォームを順に処理する

	//キャラクターの大元
	worldTransforms_[PartId::kRoot].Initialize();

	//脊椎
	worldTransforms_[PartId::kSpine].Initialize();
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];
	worldTransforms_[PartId::kSpine].translation_ = { 0,0,0 };

	//上半身
	//胸
	worldTransforms_[PartId::kChest].Initialize();
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kChest].translation_ = { 0,3,0 };
	//頭
	worldTransforms_[PartId::kHead].Initialize();
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kHead].translation_ = { 0,3.5f,0 };
	//左腕
	worldTransforms_[PartId::kArmL].Initialize();
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmL].translation_ = { -3.5f,0,0 };
	//右腕
	worldTransforms_[PartId::kArmR].Initialize();
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmR].translation_ = { 3.5f,0,0 };


	//下半身
	//尻
	worldTransforms_[PartId::kHip].Initialize();
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kHip].translation_ = { 0,-3,0 };
	//左足
	worldTransforms_[PartId::kLegL].Initialize();
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegL].translation_ = { -3.5f,-3.5f,0 };
	//右足
	worldTransforms_[PartId::kLegR].Initialize();
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegR].translation_ = { 3.5f,-3.5f,0 };

	for (int i = 0; i < _countof(worldTransforms_); i++)
	{
		worldTransforms_[i].matWorld_ = CreateMatWorld(worldTransforms_[i]);

		//行列の転送
		worldTransforms_[0].TransferMatrix();

	}
	//デバッグカメラの生成
	//debugCamera_ = new DebugCamera(1200, 800);

	//軸方向表示を有効にする
	//AxisIndicator::GetInstance()->SetVisible(true);

	//軸方向表示が参照するビュープロジェクションを指定する
	//AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//ライン描画が参照するビュープロジェクションを指定する
	//PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;
	//ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;
	//ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;
	//カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY =  3.14f /(10.0f);

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

	Vector3 move = { 1,0,0 };

	if (input_->PushKey(DIK_RIGHT))
	{
		worldTransforms_[PartId::kRoot].translation_ += move;
	}
	if (input_->PushKey(DIK_LEFT))
	{
		worldTransforms_[PartId::kRoot].translation_ -= move;
	}

	//大元から順に更新していく
	for (int i = 0; i < kNumPartId; i++)
	{
		worldTransforms_[i].matWorld_ = CreateMatWorld(worldTransforms_[i]);
		//parent_がnullじゃなかったら親のワールド行列にかける
		if (worldTransforms_[i].parent_ != nullptr)
		{
			worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
		}
		//行列の転送
		worldTransforms_[i].TransferMatrix();
	}

	//上半身回転処理

	Vector3 move2 = { 0,0.1f,0 };

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_U))
	{
		worldTransforms_[PartId::kChest].rotation_ -= move2;
	}
	else if (input_->PushKey(DIK_I))
	{
		worldTransforms_[PartId::kChest].rotation_ += move2;
	}

	//下半身回転処理

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_J))
	{
		worldTransforms_[PartId::kHip].rotation_ -= move2;
	}
	else if (input_->PushKey(DIK_K))
	{
		worldTransforms_[PartId::kHip].rotation_ += move2;
	}

	//デバック
	debugText_->SetPos(50, 120);
	debugText_->Printf(
		"translation:x(%f)", worldTransforms_[PartId::kRoot].translation_.x);
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
	for (int i = 0; i < kNumPartId; i++)
	{
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}
	///ライン描画が参照するビュープロジェクションを指定する
	//PrimitiveDrawer::GetInstance()->DrawLine3d({ 0,0,0 }, { 0,0,10 }, { 0,0,1,1 });


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
