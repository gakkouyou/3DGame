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
		Max,
	};

	void SetDrawType(DrawType _drawType);

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
};