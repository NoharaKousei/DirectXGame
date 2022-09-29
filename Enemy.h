#pragma once

#include"Model.h"
#include"EnemyBullet.h"
#include<Input.h>
#include<memory>
#include<list>



/// <summary>
/// 敵
/// </summary>

class Enemy
{
public:


	//行動フェーズ
	enum class Phase
	{
		Approach,	//接近する
		Leave,		//離脱する
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);



	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Fire();


	void AppPhaInitialize();

	//発射間隔
	static const int kFireInterval = 60;


private:

	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//速度
	Vector3 velocity_;

	//フェーズ
	Phase phase_ = Phase::Approach;

	//弾
	std::list < std::unique_ptr< EnemyBullet >> bullets_;
	//入力処理
	Input* input_ = nullptr;
	//発射タイマー
	int32_t shootTimer = 0;

};