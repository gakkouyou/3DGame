﻿#pragma once
#include "../../TerrainBase.h"

class BoundGround : public TerrainBase
{
public:
	BoundGround()				{}
	~BoundGround()	override	{}

	void Init()		override;

	void SetParam(const Param& _param) override;

private:
};