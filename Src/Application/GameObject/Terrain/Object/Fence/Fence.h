#pragma once
#include "../../TerrainBase.h"

class Fence : public TerrainBase
{
public:
	Fence()					{}
	~Fence()	override	{}

	void Init()	override;

	// 地形のパラメータセット関数
	void SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos = Math::Vector3::Zero, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero);
};