#pragma once

class ResultBackGround : public KdGameObject
{
public:
	ResultBackGround() {}
	~ResultBackGround()	override {}

	void DrawLit();
	void Init();

	void StartAnimation() { m_startAnimationFlg = true; }

private:
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	struct Color
	{
		float r = 0;
		float g = 0;
		float b = 0;
	};
	Math::Vector3 m_color;

	// スタートのRGB
	Math::Vector3 m_startColor;
	// ゴールのRBG
	Math::Vector3 m_goalColor;

	int m_frame = 200;
	float m_progress = 0;
	float m_speed = 0;

	bool m_startAnimationFlg = false;
	bool m_blackAnimationFlg = false;
};