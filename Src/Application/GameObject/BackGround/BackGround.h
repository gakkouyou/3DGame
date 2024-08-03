#pragma once

class BackGround : public KdGameObject
{
public:
	BackGround() {}
	~BackGround() override {}

	void DrawUnLit()	override;
	void Init()			override;

private:
	std::shared_ptr<KdModelData> m_spModel = nullptr;
};