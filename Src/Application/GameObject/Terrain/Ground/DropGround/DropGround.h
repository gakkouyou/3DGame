#pragma once

#include "../../TerrainBase.h"

class DropGround : public TerrainBase
{
public:
	DropGround() {}
	~DropGround()	override {}

	void Update()	override;
	void Init()		override;

	void OnHit()	override;

	// 座標をセットする
	void SetPos(const Math::Vector3& _pos) override { m_param.pos = _pos; }
	// 座標をゲットする
	Math::Vector3 GetPos()	const override { return m_param.pos; }

	// 情報をセットする
	virtual void SetParam(Math::Vector3 _startPos = Math::Vector3::Zero, Math::Vector3 _goalPos = Math::Vector3::Zero, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) override;

private:
	int m_stayCount = 0;

	// OnHitに入ったかどうかのフラグ
	bool m_onHitFlg = false;

	bool m_dropFlg	= false;

	// 消えるまでの時間
	const int	m_vanishTime	= 60;
	int			m_vanishCount	= 0;

	// 復活までの時間
	const int	m_respawnTime	= 60;
	int			m_respawnCount	= 0;

	// 生存フラグ
	bool m_aliveFlg = false;
};