#pragma once
#include "../../BaseTerrain.h"

class MoveGround : public BaseTerrain
{
public:
	MoveGround() {}
	~MoveGround()		override {}

	void Update()		override;
	void PostUpdate()	override;
	void Init()			override;

	struct Info
	{
		Math::Vector3 pos;
		Math::Vector3 startPos;
		Math::Vector3 goalPos;
		float speed;
		int stayTime;
	};

	void SetInfo(Math::Vector3 _startPos, Math::Vector3 _goalPos, float _speed, int _stayTime);
	const Info GetInfo() const { return m_info; }

	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

	// ゲームをやり直す際に呼ぶ関数
	void Reset();

private:

	Info m_info;

	// 動くか動かないか
	bool m_moveFlg = false;
	// スタート→ゴールか、ゴール→スタートか
	bool m_moveDirFlg = false;
	// スタート位置、ゴール位置で一定時間止まる
	int m_stayCnt = 0;

	// 止めるフラグ
	bool m_stopFlg = false;
};