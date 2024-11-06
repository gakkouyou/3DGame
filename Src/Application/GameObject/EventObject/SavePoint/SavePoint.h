﻿#pragma once

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
	//std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 土台のモデル
	std::shared_ptr<KdModelData> m_spBaseModel = nullptr;

	Math::Matrix m_flagMat;

	// 座標
	Math::Vector3 m_pos;

	// リスポーン座標
	Math::Vector3 m_respawnPos;

	// 角度
	float m_degAng = 90.0f;
	// 引く角度元
	const float m_baseSumDegAng = 1.0f;
	// 引く角度
	float m_sumDegAng = 1.0f;
	// 引く角度に足しこむ角度
	const float m_addSumDegAng = 0.4f;

	float m_progress = 0;
	float m_speed = 0.02f;

	const float m_maxDegAng = -45.0f;
	const float m_minDegAng = 22.5f;
	const float m_standDegAng = 0;

	enum class SituationType
	{
		NotStand,
		Up,
		Down,
		Up2,
		Stand
	};

	SituationType m_situationType = SituationType::NotStand;
};