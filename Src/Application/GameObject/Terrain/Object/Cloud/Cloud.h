#pragma once
#include "../../TerrainBase.h"

class Cloud : public TerrainBase
{
public:
	Cloud() {}
	~Cloud()	override {}

	void Update()	override;
	void Init()		override;

	// 地形のパラメータセット関数
	void SetParam(const Param& _param)	override;
};