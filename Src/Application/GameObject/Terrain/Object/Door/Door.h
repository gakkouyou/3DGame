#pragma once

#include "../../TerrainBase.h"

class Door : public TerrainBase
{
public:
	Door()										 {}
	~Door()								override {}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void Init()							override;

	void Active()						override;

	void SetParam(Param _param)			override;

private:
	bool m_activeFlg	 = false;

	Math::Vector3 m_leftPos;	// 左のドアの座標
	Math::Matrix m_leftMat;		// 左のドアの行列

	// 壁のモデル
	std::shared_ptr<KdModelData> m_spWallModel = nullptr;
	Math::Matrix m_baseMat;
};