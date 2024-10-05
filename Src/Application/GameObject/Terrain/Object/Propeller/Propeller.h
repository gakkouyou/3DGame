#pragma once

#include "../../TerrainBase.h"

class Propeller : public TerrainBase
{
public:
	Propeller() {}
	~Propeller()	override {}

	void Update()	override;
	void Init()		override;

	void SetParam(const Param& _param) override;

	void Reset()	override;

	// 動くオブジェクト
	bool IsRideable() const		override { return true; }

private:
	// 回転角度
	Math::Vector3 m_rotDegAng = Math::Vector3::Zero;
};