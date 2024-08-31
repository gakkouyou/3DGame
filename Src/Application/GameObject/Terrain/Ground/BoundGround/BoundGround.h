#pragma once
#include "../../TerrainBase.h"

class BoundGround : public TerrainBase
{
public:
	BoundGround()				{}
	~BoundGround()	override	{}

	//void Update()	override;
	void Init()		override;

	void OnHit()	override;

	void SetParam(Math::Vector3 _startPos, Math::Vector3 _goalPos = Math::Vector3::Zero, Math::Vector3 _scale = { 1, 1, 1 }, float _speed = 0, int _stayTime = 0, Math::Vector3 _degAng = Math::Vector3::Zero) override;

private:
};