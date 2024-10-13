#pragma once

#include "../../TerrainBase.h"

class SlopeGround : public TerrainBase
{
public:
	SlopeGround() {}
	~SlopeGround()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()	override;

	void SetParam(const Param& _param)	override;

	void Active()	override;

	const bool IsActive()	const { return m_activeFlg; }

	const Math::Matrix& GetMatrix() const override { return m_nonScaleMat; }

private:
	// スケール
	Math::Vector3 m_scale;
	// スケールの大きくしていく値
	const Math::Vector3 m_addScale = { 0.01f, 0.01f, 0.01f };

	int m_stayCount = 0;
	const int m_stayTime = 60;

	// 更新しているかどうか
	bool m_activeFlg = false;

	Math::Matrix m_nonScaleMat;
};