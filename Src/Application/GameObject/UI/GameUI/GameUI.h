#pragma once

class GameUI : public KdGameObject
{
public:
	GameUI()					 {}
	~GameUI()			override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	enum DrawType
	{
		Warp,
		HoldBox,
		Max,
	};

	void SetDrawType(DrawType _drawType);

	void SetPauseFlg(bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

	void Reset()	override;

private:
	struct Texture
	{
		std::shared_ptr<KdTexture> spTex = nullptr;
		Math::Vector2 pos;
		float alpha = 0;
		float addAlpha = 0;
	};
	Texture m_tex[DrawType::Max];

	UINT m_drawType = DrawType::Max;
	UINT m_oldDrawType = DrawType::Max;

	// 「P:ポーズ」
	std::shared_ptr<KdTexture> m_spPauseTex = nullptr;
	const Math::Vector2 m_pausePos = { 500, -300 };

	// ポーズフラグ
	bool m_pauseFlg = false;

	// ストップフラグ
	bool m_stopFlg = false;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/UI/GameUI/GameUI.json";

	// JSONのデータをロードする
	void DataLoad();
};