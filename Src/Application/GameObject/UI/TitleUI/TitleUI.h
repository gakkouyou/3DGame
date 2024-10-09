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
		Math::Vector3				pos;
	};

	Texture m_title;

	Texture m_pushSpace;
};