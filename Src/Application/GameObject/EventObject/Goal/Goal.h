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
	float m_moveDegAng = 0.5f;
	float m_degAng = 0;
	const float m_addMoveDegAng = 0.5f;	// ゴールした時の演出の際に、くるくる回す時の加算量

	// 正面を向く角度
	const float m_forwardDegAng = 90.0f;

	// 上下にふよふよさせる
	float m_sinAngle = 0;
	// ふよふよさせる高さ
	const float m_height = 0.5f;

	// ゴールフラグ
	bool m_goalFlg	= false;

	int m_goalStayCount	= 0;
	int m_goalStayTime	= 60;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;
};