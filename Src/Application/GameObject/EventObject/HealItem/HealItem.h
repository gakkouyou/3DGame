#pragma once

#include "../EventObjectBase.h"

class HealItem : public EventObjectBase
{
public:
	HealItem()						{}
	~HealItem()			override{}

	void Update()		override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()		override;
	void DrawBright()	override;
	void Init()			override;

	void OnHit()		override;

	void Reset()		override;

	void SetParam(const Param& _param)	override;

private:
	bool m_aliveFlg = true;
};