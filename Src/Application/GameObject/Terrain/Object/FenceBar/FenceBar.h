#pragma once
#include "../../TerrainBase.h"

class FenceBar : public TerrainBase
{
public:
	FenceBar()					{}
	~FenceBar()	override	{}

	void Init()	override;

	// 地形のパラメータセット関数
	void SetParam(const Param _param)	override;

	// 乗った際にカメラが追尾すべきかすべきでないか
	virtual bool IsCameraTracking() { return false; }
};