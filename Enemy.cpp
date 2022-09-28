#include"Enemy.h"
#include <cassert>





void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);
	model_ = model;

	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("EnemyP.png");


	//ワールド変換の初期化
	worldTransform_.Initialize();


	//キャラクターの移動ベクトル
	Vector3 move = { 0,6,0 };


	//初期座標をセット
	worldTransform_.translation_ = move;
}


void Approach(Vector3& appSpeed, WorldTransform& worldTransform_, Enemy::Phase& phase_, float z)
{

	//移動(ベクトルを加算)
	worldTransform_.translation_ += appSpeed;
	//規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < z) {
		phase_ = Enemy::Phase::Leave;
	}
}

void Leave(Vector3& leaveSpeed, WorldTransform& worldTransform_)
{
	//移動(ベクトルを加算)
	worldTransform_.translation_ += leaveSpeed;
}

void Enemy::Update()
{


	Vector3 appSpeed = { 0,0,-0.3f };
	Vector3 leaveSpeed = { 0,0,+0.3f };
	switch (phase_) {
	case Phase::Approach:
	default:
		Approach(appSpeed, worldTransform_, phase_, 0.0f);
		break;
	case Phase::Leave:
		Leave(leaveSpeed, worldTransform_);
		break;
	}

	
	//単位行列
	worldTransform_.matWorld_ = worldTransform_.CreateIdentityMatrix();
	//スケーリング行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatScale(worldTransform_.scale_);
	//回転行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatRot(worldTransform_.rotation_);
	//平行移動
	worldTransform_.matWorld_ *= worldTransform_.CreateMatTrans(worldTransform_.translation_);


	

	//行列の転送
	worldTransform_.TransferMatrix();


}




void Enemy::Draw(const ViewProjection& viewProjection)
{
	//モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);


}