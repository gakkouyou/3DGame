#pragma once

class Result : public KdGameObject
{
public:
	Result()				{}
	~Result()		override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void StageClear() { m_clearFlg = true; }
	void GameOver() { m_gameOverFlg = true; }

	const bool GetClearFinish() const { return m_clearFinishFlg; }

private:
	struct Text
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;				// 画像
		Math::Vector2				pos		= Math::Vector2::Zero;	// 座標
		float						alpha	= 0;					// アルファ値
		float						alphaAdd= 0;					// 足しこむアルファ値
	};

	Text m_clear;

	// クリア画像を出してから次のシーンに移るまでの待機時間
	const int m_clearStayTime	= 60;
	int m_clearStayCount		= 0;
	bool m_clearFinishFlg		= false;

	bool m_clearFlg		= false;
	bool m_gameOverFlg	= false;
};