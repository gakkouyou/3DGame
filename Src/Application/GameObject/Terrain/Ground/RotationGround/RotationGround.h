#pragma once
#include "../../TerrainBase.h"

class RotationGround : public TerrainBase
{
public:
	RotationGround() {}
	~RotationGround()	override {}

	void Update()	override;
	void Init()		override;

	void SetParam(Param _param) override;

	void Reset()	override;

private:
	// 回転角度
	Math::Vector3 m_rotDegAng = Math::Vector3::Zero;

	// 回転行列
	Math::Matrix m_rotMat		= Math::Matrix::Identity;
};