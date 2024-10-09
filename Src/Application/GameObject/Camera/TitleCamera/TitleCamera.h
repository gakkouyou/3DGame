#pragma once

#include "../CameraBase.h"

class TitleCamera : public CameraBase
{
public:
	TitleCamera() {}
	~TitleCamera()	override {}

	void Update()	override;
	void Init()		override;

	const bool GetAnimationFinishFlg() const { return m_animationFinishFlg; }

private:
	Math::Vector3 m_moveVec;

	bool m_shiftFlg = false;

	// 移動ベクトル
	const float m_moveSpeed = 0.15f;

	Math::Vector3 m_debugPos;

	// 座標
	Math::Vector3 m_pos;
	// 初期の行列
	Math::Matrix m_startMat;

	// ゴールの行列
	Math::Matrix m_goalMat;

	// アニメーション開始フラグ
	bool m_animationFlg = false;

	// アニメーションが終わったかどうかのフラグ
	bool m_animationFinishFlg = false;
};