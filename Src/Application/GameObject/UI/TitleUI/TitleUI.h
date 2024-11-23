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

	float m_alpha = 0.0f;
	const float m_addAlpha = 0.01f;

	bool m_startFlg = false;

	// サインカーブ用
	float m_degAng = 90;
	float m_addDegAng = 2;
	float m_pushSpaceAlpha = 0;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/UI/TitleUI/TitleUI.json";

	// JSONのデータをロードする
	void DataLoad();
};