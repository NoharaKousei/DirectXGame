﻿#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>
#include "DebugCamera.h"

//パーツID
enum PartId {
	kRoot,
	/*kSpine,
	kChest,
	kHead,
	kArmL,
	kArmR,
	kHip,
	kLegL,
	kLegR,*/

	kOver,

	kUe,
	kSita,
	kMigi,
	kHidari,
	kRoot2,
	kUe2,
	kSita2,
	kMigi2,
	kHidari2,
	
	

	kNumPartId
};


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();
	/// <summary>
	/// 初期化
	/// </summary>

	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	Sprite* sprite_ = nullptr;
	// 3Dモデル
	Model* model_ = nullptr;
	//デバックカメラ
	DebugCamera* debugCamera_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransforms_[kNumPartId];
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//値を表示したい変数
	int32_t value_ = 0;
	//カメラ上方向の角度
	float viewAngle = 0.0f;

};