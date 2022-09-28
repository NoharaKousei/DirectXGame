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




void Enemy::Update()
{

	float speed = 0.5f;
	//単位行列
	worldTransform_.matWorld_ = worldTransform_.CreateIdentityMatrix();
	//スケーリング行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatScale(worldTransform_.scale_);
	//回転行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatRot(worldTransform_.rotation_);
	//平行移動
	worldTransform_.matWorld_ *= worldTransform_.CreateMatTrans(worldTransform_.translation_);


	//座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.z -= speed;


	//行列の転送
	worldTransform_.TransferMatrix();


}




void Enemy::Draw(const ViewProjection& viewProjection)
{
	//モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);


}