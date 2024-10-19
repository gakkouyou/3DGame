#pragma once

#include "../EventObjectBase.h"

class SavePoint : public EventObjectBase
{
public:
	SavePoint()									{}
	~SavePoint()						override{}

	void Update()						override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()						override;
	void DrawUnLit()					override;
	void DrawBright()					override;
	void Init()							override;

	Math::Vector3 GetPos() const		override { return m_respawnPos; }

	void OnHit()	override;

	void SetParam(const Param& _param)	override;

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 座標
	Math::Vector3 m_pos;

	// リスポーン座標
	Math::Vector3 m_respawnPos;

	bool m_aliveFlg = true;
};