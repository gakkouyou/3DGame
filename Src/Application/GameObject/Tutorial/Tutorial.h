#pragma once

class Tutorial : public KdGameObject
{
public:
	Tutorial() {}
	~Tutorial()	override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void Display() { m_displayFlg = true; }

	void SetPauseFlg(bool _pauseFlg) override { m_pauseFlg = _pauseFlg; }

private:
	struct Texture
	{
		std::shared_ptr<KdTexture> spTex = nullptr;	// 画像
		Math::Vector2 pos;							// 座標
	};

	enum Type
	{
		MoveKey,
		MoveText,
		JumpKey,
		JumpText,
		ActionKey,
		ActionText,
		Max,
	};

	Texture m_tex[Type::Max];

	// 表示するかどうかのフラグ
	bool m_displayFlg = false;

	// アルファ値系
	float m_alpha = 0.0f;		// アルファ値
	float m_addAlpha = 0.05f;	// アルファ値に加算する数値
	float m_maxAlpha = 1.0f;	// アルファ値の上限
	float m_minAlpha = 0.0f;	// アルファ値の下限

	// 表示するまでの時間
	int m_displayCount = 0;
	const int m_displayTime = 300;

	bool m_pauseFlg = false;
};