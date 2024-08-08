#pragma once
#include "../../TerrainBase.h"

class RotationGround : public TerrainBase
{
public:
	RotationGround() {}
	~RotationGround()	override {}

	void Update()	override;
	void Init()		override;

	virtual void SetInfo(Math::Vector3 _startPos = Math::Vector3::Zero, Math::Vector3 _goalPos = Math::Vector3::Zero, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) override;

private:
	// 回転角度
	Math::Vector3 m_rotDegAng = Math::Vector3::Zero;
};