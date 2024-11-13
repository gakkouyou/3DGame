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

	Math::Vector3 m_color;
	const Math::Vector3 m_startColor = { 0.0f, 0.0f, 0.0f };
	const Math::Vector3 m_goalColor = { 1.0f, 1.0f, 1.0f };
	float m_progress = 0.0f;
	const float m_speed = 0.005f;
};