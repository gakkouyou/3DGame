#pragma once
#include "../CharacterBase.h"

class CameraBase;

class Player : public CharacterBase
{
public:
	Player()										{}
	~Player()							override	{}

	// 更新
	void Update()						override;
	void PostUpdate()					override;

	// 初期化
	void Init()							override;

	// リセット処理
	void Reset()						override;

	// カメラセット
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

private:
	// 当たり判定
	void HitJudge();
	// 地面との当たり判定
	void HitJudgeGround();
	// 当たったらイベントが発生する系の当たり判定
	void HitJudgeEvent();
	// 敵との当たり判定
	void HitEnemy();

	// オブジェクトに当たった時の反応
	//void Bound();
	

	// 今の状況
	enum SituationType
	{
		Stop	= 1 << 0,	// 止まっている
		Run		= 1 << 1,	// 走っている
		Jump	= 1 << 2,	// ジャンプしている
		Air		= 1 << 3,	// 空中にいる
		WallHit	= 1 << 4,	// 壁に触れている
	};

	// カメラのウィークポインタ
	std::weak_ptr<CameraBase> m_wpCamera;

	// 今の状況
	UINT m_situationType	= Stop;

	// ジャンプ力
	float m_jumpPow			= 0;

	// 角度
	float m_angle			= 0.0f;

	// 回転行列
	Math::Matrix m_rotMat	= Math::Matrix::Identity;

	// 当たったら一緒に動くような地形に当たった際の処理のための構造体
	struct HitMoveTerrain
	{
		Math::Matrix transMat	= Math::Matrix::Identity;	// 動く前の行列
		Math::Matrix rotMat		= Math::Matrix::Identity;	// 回転行列
		bool hitFlg				= false;					// 当たったかどうか
	};
	
	// 動く床
	HitMoveTerrain m_moveGround;
	// 回る床
	HitMoveTerrain m_rotationGround;

	// アニメーション用
	bool m_walkAnimeDirFlg = false;

	// エフェクシア用フラグ
	bool m_effectFlg = false;
};