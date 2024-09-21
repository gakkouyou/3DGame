#pragma once

#include "../../TerrainBase.h"

class Switch : public TerrainBase
{
public:
	Switch() {}
	~Switch()							override {}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void Init()							override;

	void OnHit()						override;

	void SetParam(Param _param)			override;

private:
	// 上がり下がりする量
	const float m_movePow = 0.01f;
	// 下がる限界値
	const float m_maxMove = 0.2f;

	// OnHitに入ったかどうかをチェックするフラグ
	bool m_onHitFlg = false;

	// スイッチの土台のようなモデル
	std::shared_ptr<KdModelData> m_spBaseModel	= nullptr;
	// スイッチの土台の行列
	Math::Matrix m_baseMatrix;
	// コライダー
	std::unique_ptr<KdCollider> m_pBaseCollider = nullptr;
};