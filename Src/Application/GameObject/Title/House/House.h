#pragma once

class House : public KdGameObject
{
public:
	House() {}
	~House()	override {}

	void GenerateDepthMapFromLight()	override;
	void DrawUnLit()	override;
	void Init()		override;

private:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;
};