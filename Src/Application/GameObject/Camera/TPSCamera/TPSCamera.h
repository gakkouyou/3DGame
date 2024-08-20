#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void PostUpdate()		override;
	void Init()				override;

	void Reset()			override;

	void SetGoalFlg(bool _goalFlg) { m_goalFlg = _goalFlg; }

	void SetGoalPos(Math::Vector3 _goalPos) { m_goalPos = _goalPos; }

private:
	// マップエディタモードの際に使用する座標
	Math::Vector3 m_debugPos	= Math::Vector3::Zero;
	// 移動ベクトル
	Math::Vector3 m_moveVec		= Math::Vector3::Zero;
	const float m_moveSpeed		= 0.6f;

	// キーフラグ
	bool m_shiftFlg				= false;

	// ゴールフラグ
	bool m_goalFlg				= false;

	// ゴールの座標
	Math::Vector3 m_goalPos		= Math::Vector3::Zero;
	// ゴールの処理の時のローカル行列
	Math::Vector3 m_goalLocalPos = Math::Vector3::Zero;

	void GoalProcess();
};