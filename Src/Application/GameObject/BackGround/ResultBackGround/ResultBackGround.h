#pragma once

class ResultBackGround : public KdGameObject
{
public:
	ResultBackGround() {}
	~ResultBackGround()	override {}

	void Update()	override;
	void DrawLit()	override;
	void Init()		override;

	void OrangeAnimation() { m_orangeAnimationFlg = true; }
	void BlackAnimation() { m_blackAnimationFlg = true; }

	const bool GetOrangeAnimationEnd() const { return m_orangeAnimationEndFlg; }

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

	bool m_orangeAnimationFlg = false;
	bool m_blackAnimationFlg = false;
	bool m_orangeAnimationEndFlg = false;


	float m_ambientLight = 0.6f;
	float m_halfAmbientLight;
	float m_addAmbientLight;
};