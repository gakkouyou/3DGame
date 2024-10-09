#pragma once

class House : public KdGameObject
{
public:
	House() {}
	~House()	override {}

	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	const Math::Vector3 GetBedPos() const { return m_bedPos; }

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	// 道路のモデル
	std::shared_ptr<KdModelData> m_spRoadModel = nullptr;
	Math::Matrix m_roadMat;

	// 家具のオブジェクトのモデル
	std::shared_ptr<KdModelData> m_spHouseObjectModel = nullptr;

	// ベッドの座標
	Math::Vector3 m_bedPos;
};