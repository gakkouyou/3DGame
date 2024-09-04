#pragma once

class GameUI : public KdGameObject
{
public:
	GameUI()					{}
	~GameUI()			override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

private:
	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;				// 画像
		Math::Vector2				pos		= Math::Vector2::Zero;	// 座標
	};
	Texture m_life;
};