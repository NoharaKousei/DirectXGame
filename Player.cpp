#include "Player.h"
#include "GameScene.h"
#include <cassert>



void Player::Initialize(Model* model, uint32_t textureHandle) {

	assert(model);

	model_ = Model::Create();

	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();
	
}

void Player::Update() {

	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 move = { 0, 0, 0 };

	if (input_->PushKey(DIK_RIGHT)) {

		move.x += 1.0f;
	}
	if (input_->PushKey(DIK_LEFT)) {

		move.x -= 1.0f;
	}
	if (input_->PushKey(DIK_UP)) {

		move.y += 1.0f;
	}
	if (input_->PushKey(DIK_DOWN)) {

		move.y -= 1.0f;
	}

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	
	const float kMoveLimitX = 100;
	const float kMoveLimitY = 100;

	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//�s��X�V
	worldTransform_.translation_ += move;
	worldTransform_.TransferMatrix();
}

void Player::Draw(ViewProjection& viewProjection_) {

	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

}