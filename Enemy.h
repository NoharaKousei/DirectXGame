#pragma once

#include"Model.h"
#include"EnemyBullet.h"
#include<Input.h>
#include<memory>
#include<list>

// ���@�N���X�̑O���錾
class Player;

/// <summary>
/// �G
/// </summary>

class Enemy
{
public:


	void SetPlayer(Player* player) { player_ = player; }

	//�s���t�F�[�Y
	enum class Phase
	{
		Approach,	//�ڋ߂���
		Leave,		//���E����
	};


	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);



	/// <summary>
	/// �X�V
	/// </summary>
	void Update();


	/// <summary>
	/// �`��
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// �U��
	/// </summary>
	void Fire();


	void AppPhaInitialize();

	//���ˊԊu
	static const int kFireInterval = 60;


	/// <summary>
	/// ���[���h���W���擾
	/// </summary>
	Vector3 GetWorldPosition();


private:

	//���[���h�g�����X�t�H�[��
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;
	//���x
	Vector3 velocity_;

	// ���L����
	Player* player_ = nullptr;


	//�t�F�[�Y
	Phase phase_ = Phase::Approach;

	//�e
	std::list < std::unique_ptr< EnemyBullet >> bullets_;
	//���͏���
	Input* input_ = nullptr;
	//���˃^�C�}�[
	int32_t shootTimer = 0;

};