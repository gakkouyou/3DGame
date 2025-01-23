#pragma once

class CameraChange : KdGameObject
{
public:
	CameraChange() {};
	~CameraChange()	override {};

	void Init()		override;

	void OnHit()	override;

private:

};