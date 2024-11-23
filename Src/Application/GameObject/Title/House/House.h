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
	// 家のモデル
	std::shared_ptr<KdModelData> m_spModel[3] = { nullptr, nullptr, nullptr };
	Math::Vector3 m_housePos[3];
	Math::Matrix m_houseMat[3];

	// 道路のモデル
	std::shared_ptr<KdModelData> m_spRoadModel = nullptr;
	// 道路の座標
	Math::Vector3 m_roadPos;
	// 道路の拡縮
	float m_roadScale = 0;
	// 道路の行列
	Math::Matrix m_roadMat;

	// ドアのモデル
	std::shared_ptr<KdModelData> m_spDoorModel = nullptr;
	Math::Matrix m_doorMat;

	// 家具のオブジェクトのモデル
	std::shared_ptr<KdModelData> m_spHouseObjectModel = nullptr;

	// 木のモデル
	std::shared_ptr<KdModelData> m_spTreeModel = nullptr;
	// 木の座標
	Math::Vector3 m_treePos[2];
	// 木の行列
	Math::Matrix m_treeMat[2];

	// ベッドの座標
	Math::Vector3 m_bedPos;

	bool m_openDoorFlg	= false;
	bool m_closeDoorFlg = false;

	// ドアの角度
	float m_degAng = 0;
	float m_addDegAng = 0;
	float m_maxDegAng = 0;

	// ドアを開ける音
	bool m_openSoundFlg = false;
	std::weak_ptr<KdSoundInstance> m_wpOpenSound;

	// ドアを閉める音
	bool m_closeSoundFlg = false;
	std::weak_ptr<KdSoundInstance> m_wpCloseSound;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/Title/House/House.json";

	// JSONのデータをロードする
	void DataLoad();
};