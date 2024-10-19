#pragma once
#include "../../TerrainBase.h"

class MoveGround : public TerrainBase
{
public:
	MoveGround() {}
	~MoveGround()		override {}

	void Update()		override;
	void PostUpdate()	override;
	void Init()			override;

	void Reset()		override;

	// 座標をセットする
	void SetPos(const Math::Vector3& _pos) override { m_param.pos = _pos; }
	// 座標をゲットする
	Math::Vector3 GetPos()	const override { return m_param.pos; }

	// 情報をセットする
	void SetParam(const Param& _param) override;

	// 動くオブジェクト
	bool IsRideable() const		override { return true; }
	
	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

private:
	// 動くか動かないか
	bool m_moveFlg = false;
	// スタート→ゴールか、ゴール→スタートか
	bool m_moveDirFlg = false;
	// スタート位置、ゴール位置で一定時間止まる
	int m_stayCnt = 0;

	// 止めるフラグ
	bool m_stopFlg = false;
};