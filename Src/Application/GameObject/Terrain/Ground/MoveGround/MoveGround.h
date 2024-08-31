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

	// 座標をセットする
	void SetPos(const Math::Vector3& _pos) override { m_param.pos = _pos; }
	// 座標をゲットする
	Math::Vector3 GetPos()	const override { return m_param.pos; }

	// 情報をセットする
	virtual void SetParam(Math::Vector3 _startPos = Math::Vector3::Zero, Math::Vector3 _goalPos = Math::Vector3::Zero, Math::Vector3 _scale = { 1, 1, 1 }, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) override;

	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

	// ゲームをやり直す際に呼ぶ関数
	void Reset();

private:
	// 動くか動かないか
	bool m_moveFlg = false;
	// スタート→ゴールか、ゴール→スタートか
	bool m_moveDirFlg = false;
	// スタート位置、ゴール位置で一定時間止まる
	int m_stayCnt = 0;

	// 止めるフラグ
	bool m_stopFlg = false;

	// SetParamが呼ばれたかどうか
	bool m_setParamFlg = false;
};