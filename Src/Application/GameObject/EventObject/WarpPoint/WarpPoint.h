#pragma once

#include "../EventObjectBase.h"

class WarpPoint : public EventObjectBase
{
public:
	WarpPoint() {}
	~WarpPoint()	override {}

	void Update()		override;
	void DrawUnLit()	override;
	void DrawBright()	override;
	void Init()			override;

	// 出口の座標を返す
	Math::Vector3 GetPos() const override { return m_outPos; }

	void OnHit()	override;

	void SetParam(const Param& _param)	override;

private:
	// 入口のモデル
	std::shared_ptr<KdModelData> m_spInModel	= nullptr;
	// 出口のモデル
	std::shared_ptr<KdModelData> m_spOutModel	= nullptr;

	Math::Matrix m_outMat;

	// 入口の座標
	Math::Vector3 m_inPos;
	// 出口の座標
	Math::Vector3 m_outPos;

	float m_degAng = 0;

	// エフェクシア用フラグ
	bool m_effectFlg = false;
	// エフェクシア用ウィークポインタ
	std::weak_ptr<KdEffekseerObject> m_wpEffekseer;
};