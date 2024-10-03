#pragma once

class TitleBackGround : public KdGameObject
{
public:
	TitleBackGround() {}
	~TitleBackGround()	override {}

	void DrawUnLit();
	void Init();

private:
	std::shared_ptr<KdModelData> m_spModel = nullptr;
};