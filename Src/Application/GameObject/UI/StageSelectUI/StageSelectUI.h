#pragma once

class StageSelectTexture;

class StageSelectUI : public KdGameObject
{
public:
	StageSelectUI()				{}
	~StageSelectUI()	override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void OnHit()		override;

private:
	// サインカーブ構造体
	struct SinCurve
	{
		float	degAng		= 0;	// 角度
		float	maxDegAng	= 0;	// 最大角度
		float	minDegAng	= 0;	// 最小角度
		float	addAng		= 0;	// 足しこむ角度
	};

	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;				// 画像
		Math::Vector2				pos		= Math::Vector2::Zero;	// 座標
		float						alpha	= 0;					// アルファ値
		SinCurve					sinCurve;						// サインカーブ用
	};

	// ステージ
	Texture m_stage;
	// 数字
	Texture m_number;
	// "Clear!"
	Texture m_clear;
	// 「ステージにはいる」の画像
	Texture m_stageIn;

	// OnHitに入ったかどうか
	bool m_onHitFlg = false;
};