#pragma once

#include "../EventObjectBase.h"

class FinalGoal : public EventObjectBase
{
public:
	FinalGoal()				{}
	~FinalGoal()	override{}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	void OnHit()	override;

	void SetParam(const Param& _param)	override;

private:
	Math::Vector3 m_pos = Math::Vector3::Zero;

	// スケール
	const float m_scale = 0.5f;

	// 少し回転させる
	float m_moveDegAng = 1.0f;
	float m_degAng = 0;

	// 上下にふよふよさせる
	float m_sinAngle = 0;

	// ゴールフラグ
	bool m_goalFlg = false;

	int m_goalStayCount = 0;
	int m_goalStayTime = 60;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;

	// ポーズ画面かどうか
	bool m_pauseFlg = false;

	std::weak_ptr<KdSoundInstance3D> m_wpSound;
	bool m_flg = false;
};