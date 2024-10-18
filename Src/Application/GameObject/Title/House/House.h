#pragma once

class House : public KdGameObject
{
public:
	House() {}
	~House()		override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	const Math::Vector3 GetBedPos() const { return m_bedPos; }

	void OpenDoor() { m_openDoorFlg = true; }
	void CloseDoor() { m_closeDoorFlg = true; }

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 道路のモデル
	std::shared_ptr<KdModelData> m_spRoadModel = nullptr;
	Math::Matrix m_roadMat;

	// ドアのモデル
	std::shared_ptr<KdModelData> m_spDoorModel = nullptr;
	Math::Matrix m_doorMat;

	// 家具のオブジェクトのモデル
	std::shared_ptr<KdModelData> m_spHouseObjectModel = nullptr;

	// ベッドの座標
	Math::Vector3 m_bedPos;

	bool m_openDoorFlg	= false;
	bool m_closeDoorFlg = false;

	// ドアの角度
	float m_degAng = 0;
	const float m_addDegAng = 2.0f;
	const float m_maxDegAng = 90;

	// ドアを開ける音
	bool m_openSoundFlg = false;
	std::weak_ptr<KdSoundInstance> m_wpOpenSound;

	// ドアを閉める音
	bool m_closeSoundFlg = false;
	std::weak_ptr<KdSoundInstance> m_wpCloseSound;
};