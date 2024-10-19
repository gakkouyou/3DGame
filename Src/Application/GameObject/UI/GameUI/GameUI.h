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

	// クリアしたかどうか
	void SetClearFlg(bool _clearFlg) { m_clearFlg = _clearFlg; }

private:
	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;				// 画像
		Math::Vector2				pos		= Math::Vector2::Zero;	// 座標
	};
	// ライフ
	Texture m_life;
	const int m_lifeTexInterval = 70;
	// 中がないライフ
	Texture m_emptyLife;

	// 残機のアイコン
	Texture m_stockIcon;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	bool m_clearFlg = false;
};