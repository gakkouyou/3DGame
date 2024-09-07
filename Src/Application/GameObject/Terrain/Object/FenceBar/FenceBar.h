#pragma once
#include "../../TerrainBase.h"

class FenceBar : public TerrainBase
{
public:
	FenceBar()					{}
	~FenceBar()	override	{}

	void Init()	override;

	// 地形のパラメータセット関数
	void SetParam(Param _param)	override;
};