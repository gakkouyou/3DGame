#pragma once

#include "../CarryObjectBase.h"

class BoxEnemy : public CarryObjectBase
{
public:
	BoxEnemy() {}
	~BoxEnemy()	override {}

	void Update()				override;
	void PostUpdate()			override;
	void Init()					override;

	void CarryFlg(bool _carryFlg) override;

	void SetParam(Param _param)	override;

private:
	// 当たり判定
	void HitJudge();

	// 角度
	float m_degAng = 0;

	enum Dir
	{
		RightFront,
		RightBack,
		LeftFront,
		LeftBack,
		Up,
		Down,
		Max
	};
	Math::Vector3 m_edgeBasePos[Max] = {};
	Math::Vector3 m_edgePos[Max] = {};

	Math::Vector3 m_centerPos;

	// 下に落ちたら、リスポーンさせる
	float m_underLine = -30.0f;

	Math::Vector3 m_oldPos;
};