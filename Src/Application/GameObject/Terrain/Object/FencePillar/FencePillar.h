#pragma once
#include "../../TerrainBase.h"

class FencePillar : public TerrainBase
{
public:
	FencePillar() {}
	~FencePillar()	override {}

	void Init()	override;

	// 地形のパラメータセット関数
	void SetParam(const Param& _param)	override;
};