#pragma once
#include"Model.h"
#include"WorldTransform.h"
#include "Input.h"
#include "DebugText.h"


/// <summary>
/// 自キャラ
/// </summary>
class Player
{
public:	//メンバー関数

/// <summary>
/// 初期化
/// </summary>
/// <param name="model">モデル</param>
/// <param name="textrueHandle">テクスチャハンドル</param>
	void Initialize(Model* model, uint32_t textrueHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション(参照渡し)</param>
	void Draw(ViewProjection& viewProjection);
private:	//メンバー変数

	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	//ViewProjection viewProjection_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャ
	uint32_t textureHandle_ = 0u;
	//入力処理
	Input* input_ = nullptr;
	//デバッグテキスト
	DebugText* debugText_ = nullptr;
};