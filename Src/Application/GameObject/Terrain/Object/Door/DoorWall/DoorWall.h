#pragma once

#include "../../../TerrainBase.h"

class DoorWall : public TerrainBase
{
public:
	DoorWall() {}
	~DoorWall()				override {}

	void Init()					override;

	void SetParam(Param _param)	override;

	void SetRightDoor(const std::weak_ptr<TerrainBase>& _wpRightDoor) { m_wpRightDoor = _wpRightDoor; }
	void SetLeftDoor(const std::weak_ptr<TerrainBase>& _wpLeftDoor) { m_wpLeftDoor = _wpLeftDoor; }

private:
	std::weak_ptr<TerrainBase> m_wpRightDoor;
	std::weak_ptr<TerrainBase> m_wpLeftDoor;
};