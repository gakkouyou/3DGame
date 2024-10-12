#pragma once

class TitleUI : public KdGameObject
{
public:
	TitleUI() {}
	~TitleUI()	override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

private:
	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex = nullptr;
		Math::Vector2				pos;
	};

	Texture m_title;

	Texture m_pushSpace;

	bool m_deleteFlg = false;

	float m_alpha = 1.0f;
};