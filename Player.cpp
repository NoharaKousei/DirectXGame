#include"Player.h"
#include <cassert>


//旋回
void Player::Rotate(Input* input_, float& y)
{
	if (input_->PushKey(DIK_Z))
	{
		//Y軸まわりの角度を増加
		y += 0.03f;
	}
	else if (input_->PushKey(DIK_C))
	{
		//Y軸まわりの角度を減少
		y -= 0.03f;
	}
}


//攻撃
void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = CrossV3M4(velocity, worldTransform_.CreateMatRot(worldTransform_.rotation_));


		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet>newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);

		//弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

Vector3 CrossV3M4(Vector3& v, const Matrix4& m)
{
	v.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
	v.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
	v.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
	return v;
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
	//単位行列
	worldTransform_.matWorld_ = worldTransform_.CreateIdentityMatrix();
	//スケーリング行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatScale(worldTransform_.scale_);
	//回転行列
	worldTransform_.matWorld_ *= worldTransform_.CreateMatRot(worldTransform_.rotation_);
	//平行移動
	worldTransform_.matWorld_ *= worldTransform_.CreateMatTrans(worldTransform_.translation_);

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});


	//旋回
	Rotate(input_, worldTransform_.rotation_.y);

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


	//攻撃
	Attack();


	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}


}

void Player::Draw(ViewProjection& viewprojection) {

	//3Dモデルを描画
	model_->Draw(worldTransform_, viewprojection, textureHandle_);

	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewprojection);
	}

}