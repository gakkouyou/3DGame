#pragma once
#include "../BaseCharacter.h"

class CameraBase;

class Player : public BaseCharacter
{
public:
	Player()										{}
	~Player()							override	{}

	// 更新
	void Update()						override;
	void PostUpdate()					override;

	// 描画
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;

	// 初期化
	void Init()							override;

	// カメラセット
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }

private:
	// 当たり判定
	void HitJudge();

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

	// 動く床の動く前の行列
	Math::Matrix m_beforeMoveMat	= Math::Matrix::Identity;
	// 動く床に当たったかどうか
	bool m_hitMoveGroundFlg			= false;

	// アニメーション用
	bool m_walkAnimeDirFlg = false;

	// エフェクシア用フラグ
	bool m_effectFlg = false;
};