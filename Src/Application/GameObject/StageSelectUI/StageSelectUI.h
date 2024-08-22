﻿#pragma once

class StageSelectTexture;

class StageSelectUI : public KdGameObject
{
public:
	StageSelectUI()				{}
	~StageSelectUI()	override{}

	void Update();
	void DrawSprite();
	void Init();

	void SetStageSelectTexture(std::shared_ptr<StageSelectTexture> _spStage) { m_wpStage = _spStage; }

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

	// 三角の画像
	Texture m_triangle;
	// ステージ
	Texture m_stage;
	// 数字
	Texture m_number;
	// ステージ数の枠
	Texture m_frame;
	// "PUSH SPACE"
	Texture m_space;
	// 白い画像
	Texture m_white;
	// "Clear!"
	Texture m_clear;

	// ステージ数とか持ってるクラス
	std::weak_ptr<StageSelectTexture> m_wpStage;
};