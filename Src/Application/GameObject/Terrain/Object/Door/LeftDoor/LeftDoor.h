#pragma once

#include "../../../TerrainBase.h"

class LeftDoor : public TerrainBase
{
public:
	LeftDoor() {}
	~LeftDoor()				override {}

	void Update()				override;
	void Init()					override;

	void Active()				override;

	void SetParam(const Param _param)	override;

	void SetRightDoor(const std::weak_ptr<TerrainBase>& _wpRightDoor) { m_wpRightDoor = _wpRightDoor; }
	void SetDoorWall(const std::weak_ptr<TerrainBase>& _wpDoorWall) { m_wpDoorWall = _wpDoorWall; }

private:
	bool m_activeFlg = false;

	std::weak_ptr<TerrainBase> m_wpRightDoor;
	std::weak_ptr<TerrainBase> m_wpDoorWall;
};