#pragma once

#include "../EventObjectBase.h"

class Goal : public EventObjectBase
{
public:
	Goal()									{}
	~Goal()							override{}

	void Update()					override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()					override;
	void Init()						override;

	void OnHit()	override;

	// リセット処理
	void Reset()	override;

	void SetPos(const Math::Vector3& _pos)	override { m_pos = _pos; }
	Math::Vector3 GetPos() const			override { return m_pos; }

	void SetParam(const Param& _param)	override;

private:
	Math::Vector3 m_pos	= Math::Vector3::Zero;

	// スケール
	const float m_scale = 0.5f;

	// 少し回転させる
	float m_moveDegAng = 1.0f;
	float m_degAng = 0;

	// 上下にふよふよさせる
	float m_sinAngle = 0;

	// ゴールフラグ
	bool m_goalFlg	= false;

	int m_goalStayCount	= 0;
	int m_goalStayTime	= 60;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;
};