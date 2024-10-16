#pragma once

#include "../CameraBase.h"

class ResultCamera : public CameraBase
{
public:
	ResultCamera() {}
	~ResultCamera()	override {}

	void Init()		override;

private:
	const Math::Vector3 m_pos = { 0.0f, 10.0f, -20.0f };
	const float m_degAng = 20.0f;
};