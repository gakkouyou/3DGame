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
	// 初クリア時の演出用
	float m_scale = 0;					// 拡縮
	float m_addScale = 0.01f;			// 拡縮の加算値
	const float m_addAddScale = 0.01f;	// 拡縮の加算値の加算値(大きくしていく値を大きくする)
	const float m_maxScale = 1.4f;		// 上限の拡縮

	bool m_sumFlg = false;				// 大きくするかどうか

	int m_stayCount = 0;
	const int m_stayTime = 60;

	// 更新しているかどうか
	bool m_activeFlg = false;

	Math::Matrix m_nonScaleMat;
};