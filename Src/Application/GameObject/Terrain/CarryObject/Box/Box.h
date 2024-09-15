#pragma once

#include "../CarryObjectBase.h"

class Box : public CarryObjectBase
{
public:
	Box() {}
	~Box()						override {}

	void Update()				override;
	void PostUpdate()			override;
	void Init()					override;

	void SetParam(Param _param)	override;

private:
	// 当たり判定
	void HitJudge();

	// 角度
	float m_degAng	= 0;
};