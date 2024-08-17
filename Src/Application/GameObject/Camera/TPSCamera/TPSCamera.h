#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void PostUpdate()		override;
	void Init()				override;

private:
	// マップエディタモードの際に使用する座標
	Math::Vector3 m_debugPos	= Math::Vector3::Zero;
	// 移動ベクトル
	Math::Vector3 m_moveVec		= Math::Vector3::Zero;
	const float m_moveSpeed		= 0.6f;

	// キーフラグ
	bool m_shiftFlg;
};