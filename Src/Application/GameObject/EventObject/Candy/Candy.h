#pragma once

#include "../EventObjectBase.h"

class Candy : public EventObjectBase
{
public:
	Candy()										{}
	~Candy()							override{}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void Init()							override;

	void OnHit()	override;

private:
	// 拡縮
	const float m_scale = 1.0f;

	// 角度
	float m_degAng = 0;
	// 回転角度
	const float m_moveDegAng = 2.0f;
};