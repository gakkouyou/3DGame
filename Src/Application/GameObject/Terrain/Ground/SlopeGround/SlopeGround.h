#pragma once

#include "../../TerrainBase.h"

class SlopeGround : public TerrainBase
{
public:
	SlopeGround() {}
	~SlopeGround()	override {}

	void Init()	override;

	void SetParam(const Param& _param)	override;

	void Active()	override;
};