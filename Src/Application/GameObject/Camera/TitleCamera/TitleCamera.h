#pragma once

#include "../CameraBase.h"

class TitleCamera : public CameraBase
{
public:
	TitleCamera() {}
	~TitleCamera()	override {}

	void Update()	override;
	void Init()		override;

private:
	Math::Vector3 m_moveVec;

	bool m_shiftFlg = false;

	// 移動ベクトル
	const float m_moveSpeed = 0.15f;

	Math::Vector3 m_debugPos;
};