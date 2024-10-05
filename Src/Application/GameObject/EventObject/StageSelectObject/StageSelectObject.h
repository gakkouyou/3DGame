#pragma once

#include "../EventObjectBase.h"

class StageSelectObject : public EventObjectBase
{
public:
	StageSelectObject() {}
	~StageSelectObject()	override {}

	void Update()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetParam(const Param& _param)	override;

	void OnHit()	override;

private:
};