#pragma once

class PlayerSmoke : public KdGameObject
{
public:
	PlayerSmoke() {}
	~PlayerSmoke()		override {}

	void Update()		override;
	void DrawLit()	override;
	void Init()			override;

private:
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	float m_alpha = 1.0f;
};