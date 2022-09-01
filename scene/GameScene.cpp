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
	worldTransforms_[PartId::kRoot].translation_ = { 24,-12,0 };

	//上
	worldTransforms_[PartId::kUe].Initialize();
	worldTransforms_[PartId::kUe].parent_ = &worldTransforms_[PartId::kRoot];
	worldTransforms_[PartId::kUe].translation_ = { 0,0,0 };

	//下
	worldTransforms_[PartId::kSita].Initialize();
	worldTransforms_[PartId::kSita].parent_ = &worldTransforms_[PartId::kUe];
	worldTransforms_[PartId::kSita].translation_ = { 0,-4,0 };

	//右
	worldTransforms_[PartId::kMigi].Initialize();
	worldTransforms_[PartId::kMigi].parent_ = &worldTransforms_[PartId::kUe];
	worldTransforms_[PartId::kMigi].translation_ = { 2,-2,0 };

	//右
	worldTransforms_[PartId::kHidari].Initialize();
	worldTransforms_[PartId::kHidari].parent_ = &worldTransforms_[PartId::kUe];
	worldTransforms_[PartId::kHidari].translation_ = { -2,-2,0 };

	worldTransforms_[PartId::kRoot2].Initialize();
	worldTransforms_[PartId::kRoot2].translation_ = { 800,0,0 };

	//上2
	worldTransforms_[PartId::kUe2].Initialize();
	worldTransforms_[PartId::kUe2].parent_ = &worldTransforms_[PartId::kRoot2];
	worldTransforms_[PartId::kUe2].translation_ = { 0,0,0 };

	//下2
	worldTransforms_[PartId::kSita2].Initialize();
	worldTransforms_[PartId::kSita2].parent_ = &worldTransforms_[PartId::kUe2];
	worldTransforms_[PartId::kSita2].translation_ = { 0,-4,0 };

	//右2
	worldTransforms_[PartId::kMigi2].Initialize();
	worldTransforms_[PartId::kMigi2].parent_ = &worldTransforms_[PartId::kUe2];
	worldTransforms_[PartId::kMigi2].translation_ = { 2,-2,0 };

	//右2
	worldTransforms_[PartId::kHidari2].Initialize();
	worldTransforms_[PartId::kHidari2].parent_ = &worldTransforms_[PartId::kUe2];
	worldTransforms_[PartId::kHidari2].translation_ = { -2,-2,0 };



	////脊椎
	//worldTransforms_[PartId::kSpine].Initialize();
	//worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];
	//worldTransforms_[PartId::kSpine].translation_ = { 0,0,0 };

	////上半身
	////胸
	//worldTransforms_[PartId::kChest].Initialize();
	//worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];
	//worldTransforms_[PartId::kChest].translation_ = { 0,2.0f,0 };
	////頭
	//worldTransforms_[PartId::kHead].Initialize();
	//worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	//worldTransforms_[PartId::kHead].translation_ = { 0,2.0f,0 };
	////左腕
	//worldTransforms_[PartId::kArmL].Initialize();
	//worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	//worldTransforms_[PartId::kArmL].translation_ = { -2.0f,0,0 };
	////右腕
	//worldTransforms_[PartId::kArmR].Initialize();
	//worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	//worldTransforms_[PartId::kArmR].translation_ = { 2.0f,0,0 };


	////下半身
	////尻
	//worldTransforms_[PartId::kHip].Initialize();
	//worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];
	//worldTransforms_[PartId::kHip].translation_ = { 0,-2,0 };
	////左足
	//worldTransforms_[PartId::kLegL].Initialize();
	//worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	//worldTransforms_[PartId::kLegL].translation_ = { -2.0f,-2.0f,0 };
	////右足
	//worldTransforms_[PartId::kLegR].Initialize();
	//worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	//worldTransforms_[PartId::kLegR].translation_ = { 2.0f,-2.0f,0 };

	worldTransforms_[PartId::kOver].Initialize();
	//worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kOver].translation_ = { 0.0f,2000.0f,-10 };

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

	Vector3 move = { 1.0f,1.0f,0.0f };

	int p = 0;
	int d = 0;

	if (worldTransforms_[PartId::kOver].translation_.x <= 1000)
	{
		worldTransforms_[PartId::kOver].translation_.x++;
	}

	if (worldTransforms_[PartId::kOver].translation_.x == 1001)
	{
		debugText_->SetPos(1100, 300);
		debugText_->Printf(
			"Game Over");
	}

	if (input_->PushKey(DIK_D))
	{
		worldTransforms_[PartId::kRoot].translation_.x += move.x;
	}
	if (input_->PushKey(DIK_A) && d == 0)
	{
		worldTransforms_[PartId::kRoot].translation_.x -= move.x;
		p++;
	}



	if (input_->PushKey(DIK_W))
	{
		worldTransforms_[PartId::kRoot].translation_.y += move.y;
	}
	if (input_->PushKey(DIK_S))
	{
		worldTransforms_[PartId::kRoot].translation_.y -= move.y;
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


	//自分回転処理
	Vector3 move2 = { 0.0f,0.1f,0.0f };

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_F))
	{
		worldTransforms_[PartId::kUe].rotation_ -= move2;
	}
	
	else if (input_->PushKey(DIK_G))
	{
		worldTransforms_[PartId::kUe].rotation_ += move2;
	}

	

	/*if (i == 0)
	{
		worldTransforms_[PartId::kOver].translation_.y -= move.y;
	}
	if (worldTransforms_[PartId::kOver].translation_.y  >= -500)
	{
		worldTransforms_[PartId::kOver].translation_.y = 0;
		r = 0;
	}
	if (r == 0)
	{
		debugText_->SetPos(10, 600);
		debugText_->Printf("Game Over  Rkey Reset");
	}*/

	




	////CPU回転処理
	////押した方向で移動ベクトルを変更
	//if (d == 1)
	//{
	//	worldTransforms_[PartId::kUe2].rotation_ -= move2;
	//}
	//

	//if (input_->PushKey(DIK_K))
	//{
	//	worldTransforms_[PartId::kUe2].rotation_ += move2;
	//}

	
	
	//デバック
	debugText_->SetPos(1100, 120);
	debugText_->Printf(
		"Wkey  Up");
	debugText_->SetPos(1100, 140);
	debugText_->Printf(
		"Akey  Left");
	debugText_->SetPos(1100, 160);
	debugText_->Printf(
		"Skey  Down");
	debugText_->SetPos(1100, 180);
	debugText_->Printf(
		"Dkey  Right");
	debugText_->SetPos(1100, 180);
	debugText_->Printf(
		"Dkey  Right");
	debugText_->SetPos(1100, 200);
	debugText_->Printf(
		"Fkey  Left Turn");
	debugText_->SetPos(1100, 220);
	debugText_->Printf(
		"Gkey  Right Turn");
	debugText_->SetPos(1100, 240);
	debugText_->Printf(
		"Rkey  Reset");
	debugText_->SetPos(1100, 280);
	debugText_->Printf(
		"time %f", worldTransforms_[PartId::kOver].translation_.x);

	/*debugText_->SetPos(50, 120);
	debugText_->Printf(
		"translation:x(%f)", worldTransforms_[PartId::kRoot].translation_.x);

	debugText_->SetPos(50, 140);
	debugText_->Printf(
		"kOver(%f)", worldTransforms_[PartId::kOver].translation_.y);

	debugText_->SetPos(50, 160);
	debugText_->Printf(
		"y(%f)", worldTransforms_[PartId::kRoot].translation_.y);

	debugText_->SetPos(50, 180);
	debugText_->Printf(
		"r(%f)", worldTransforms_[PartId::kUe].rotation_.y);*/

	debugText_->SetPos(0, 10);
	debugText_->Printf("-----------------------------------------------------------------------------------------------------------------------------------------------");

	debugText_->SetPos(0, 650);
	debugText_->Printf("-----------------------------------------------------------------------------------------------------------------------------------------------");

	if (15 <= worldTransforms_[PartId::kRoot].translation_.x <= 30)
	{

		debugText_->SetPos(1000, 500);
		debugText_->Printf("1, Right Turn  X  4");


		if (worldTransforms_[PartId::kUe].rotation_.y >=24.0f)
		{
			debugText_->SetPos(1010, 520);
			debugText_->Printf("Clear");
			
		}

	}

	if (worldTransforms_[PartId::kRoot].translation_.x == 13)
	{
		worldTransforms_[PartId::kUe].rotation_.y = 0.0f;
	}

	debugText_->SetPos(500, 500);
	debugText_->Printf("2, Right Turn  X  8");

	if (-10 <= worldTransforms_[PartId::kRoot].translation_.x <= 10 && worldTransforms_[PartId::kRoot].translation_.y <= 0)
	{
		if (worldTransforms_[PartId::kUe].rotation_.y >= 48.0f)
		{
			debugText_->SetPos(510, 520);
			debugText_->Printf("Clear");

		}

	}

	debugText_->SetPos(10, 500);
	debugText_->Printf("3, Left Turn  X  4");

	if (-25 >= worldTransforms_[PartId::kRoot].translation_.x >= -40 && worldTransforms_[PartId::kRoot].translation_.y <= 0)
	{
		if (worldTransforms_[PartId::kUe].rotation_.y <= -24.0f)
		{
			debugText_->SetPos(20, 520);
			debugText_->Printf("Clear");

		}

	}

	debugText_->SetPos(10, 60);
	debugText_->Printf("4, Left Turn  X  2");

	if (-25 >= worldTransforms_[PartId::kRoot].translation_.x >= -40 && worldTransforms_[PartId::kRoot].translation_.y >= 0)
	{
		if (worldTransforms_[PartId::kUe].rotation_.y <= -12.0f)
		{
			debugText_->SetPos(30, 80);
			debugText_->Printf("Clear");

		}
	}

	if (worldTransforms_[PartId::kRoot].translation_.x == -15)
	{
		worldTransforms_[PartId::kUe].rotation_.y = 0.0f;
	}

	if (worldTransforms_[PartId::kRoot].translation_.y == -2)
	{
		worldTransforms_[PartId::kUe].rotation_.y  = 0.0f;
	}
	
	debugText_->SetPos(500, 60);
	debugText_->Printf("5, Right Turn  X  6");

	if (-10 <= worldTransforms_[PartId::kRoot].translation_.x <= 10 && worldTransforms_[PartId::kRoot].translation_.y >= 0)
	{
		if (worldTransforms_[PartId::kUe].rotation_.y >= 36.0f)
		{
			debugText_->SetPos(520, 80);
			debugText_->Printf("Goal");

		}
	}

	if (input_->PushKey(DIK_R))
	{
		worldTransforms_[PartId::kRoot].translation_.x = 24.0f;
		worldTransforms_[PartId::kRoot].translation_.y = -12.0f;
		worldTransforms_[PartId::kUe].rotation_.y = 0.0f;
		worldTransforms_[PartId::kOver].translation_.x = 0.0f;
	}

	/*if (worldTransforms_[PartId::kRoot].translation_.x == 20.0f)
	{
		debugText_->SetPos(10, 651);
		debugText_->Printf("|");
		worldTransforms_[PartId::kUe].rotation_.y = 0.0f;
	}*/
	
	
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
