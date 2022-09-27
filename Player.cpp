#include"Player.h"
#include <cassert>
//05.02


//スケーリング行列
Matrix4 CreateMatScale(Vector3 scale) {
	Matrix4 matScale;
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}
//回転行列
Matrix4 CreateMatRot(Vector3 rotation) {
	//合成用回転行列を宣言
	Matrix4 matRot;
	//各軸用回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;

	matRotZ.m[0][0] = cos(rotation.z);
	matRotZ.m[0][1] = sin(rotation.z);
	matRotZ.m[1][0] = -sin(rotation.z);
	matRotZ.m[1][1] = cos(rotation.z);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cos(rotation.x);
	matRotX.m[1][2] = sin(rotation.x);
	matRotX.m[2][1] = -sin(rotation.x);
	matRotX.m[2][2] = cos(rotation.x);
	matRotX.m[3][3] = 1;

	matRotY.m[0][0] = cos(rotation.y);
	matRotY.m[0][2] = -sin(rotation.y);
	matRotY.m[1][1] = 1;
	matRotY.m[2][0] = sin(rotation.y);
	matRotY.m[2][2] = cos(rotation.y);
	matRotY.m[3][3] = 1;

	//各軸の回転行列を合成
	matRot = matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	return matRot;
}
//平行移動
Matrix4 CreateMatTrans(Vector3 translation) {
	//平行移動を宣言
	Matrix4 matTrans = MathUtility::Matrix4Identity();

	matTrans.m[0][0] = 1;
	matTrans.m[1][1] = 1;
	matTrans.m[2][2] = 1;
	matTrans.m[3][0] = translation.x;
	matTrans.m[3][1] = translation.y;
	matTrans.m[3][2] = translation.z;
	matTrans.m[3][3] = 1;

	return matTrans;
}
//単位行列
Matrix4 CreateIdentityMatrix()
{
	Matrix4 Identity;
	//単位行列を代入
	Identity.m[0][0] = 1.0f;
	Identity.m[1][1] = 1.0f;
	Identity.m[2][2] = 1.0f;
	Identity.m[3][3] = 1.0f;
	return Identity;
}



void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {
	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };
	float moveSpeed = 0.1f;

	if (input_->PushKey(DIK_RIGHT)) {
		move.x += moveSpeed;
	}
	else if (input_->PushKey(DIK_LEFT)) {
		move.x -= moveSpeed;
	}
	if (input_->PushKey(DIK_UP)) {
		move.y += moveSpeed;
	}
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= moveSpeed;
	}

	worldTransform_.translation_ += move;
	worldTransform_.matWorld_ = CreateIdentityMatrix();
	worldTransform_.matWorld_ *= CreateMatScale(worldTransform_.scale_);
	worldTransform_.matWorld_ *= CreateMatRot(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= CreateMatTrans(worldTransform_.translation_);

	//移動限界処理
	const float kMoveLimitX = 20;
	const float kMoveLimitY = 15;

	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//行列の転送
	worldTransform_.TransferMatrix();

	debugText_->SetPos(50, 50);
	debugText_->Printf("PosX:%f", worldTransform_.translation_.x);
	debugText_->SetPos(50, 70);
	debugText_->Printf("PosY:%f", worldTransform_.translation_.y);
}

void Player::Draw(ViewProjection& viewprojection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewprojection, textureHandle_);
}