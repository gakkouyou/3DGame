#pragma once

class Player;

class GameUI : public KdGameObject
{
public:
	GameUI()					{}
	~GameUI()			override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// プレイヤーをセット
	void SetPlayer(const std::shared_ptr<Player>& _spPlayer) { m_wpPlayer = _spPlayer; }

private:
	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;				// 画像
		Math::Vector2				pos		= Math::Vector2::Zero;	// 座標
	};
	Texture m_life;
	const int m_lifeTexInterval = 70;
	Texture m_emptyLife;

	std::weak_ptr<Player> m_wpPlayer;
};