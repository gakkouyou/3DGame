#pragma once

#include "../../../TerrainBase.h"

class LeftDoor;
class DoorWall;

class RightDoor : public TerrainBase
{
public:
	RightDoor()							 {}
	~RightDoor()				override {}

	void Update()				override;
	void Init()					override;

	void Active()				override;

	void SetParam(Param _param)	override;

	const std::weak_ptr<LeftDoor>& GetLeftDoor() const { return m_wpLeftDoor; }
	const std::weak_ptr<DoorWall>& GetDoorWall() const { return m_wpDoorWall; }

private:
	bool m_activeFlg = false;

	std::weak_ptr<LeftDoor> m_wpLeftDoor;
	std::weak_ptr<DoorWall> m_wpDoorWall;
};