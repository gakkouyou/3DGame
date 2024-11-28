#pragma once

class Result : public KdGameObject
{
public:
	Result()				{}
	~Result()		override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void Reset()		override;

	void StageClear() { m_clearFlg = true; }
	void GameOver() { m_gameOverFlg = true; }

	void SetGoal()		override { m_goalFlg = true; }

	const bool GetClearFinish() const { return m_clearFinishFlg; }

private:
	struct Text
	{
		std::shared_ptr<KdTexture>	spTex		= nullptr;				// 画像
		Math::Vector2				pos			= Math::Vector2::Zero;	// 座標
		Math::Vector2				move		= Math::Vector2::Zero;	// 移動量
		float						alpha		= 0;					// アルファ値
		float						alphaAdd	= 0;					// 足しこむアルファ値
	};

	// クリア
	Text m_clear;

	// クリア画像を出してから次のシーンに移るまでの待機時間
	const int m_clearStayTime	= 60;
	int m_clearStayCount		= 0;
	bool m_clearFinishFlg		= false;

	bool m_clearFlg		= false;
	bool m_gameOverFlg	= false;

	// ミス
	Text m_miss;

	// ２回バウンドさせる
	int m_boundCnt = 0;
	const int m_boundMax	= 2;

	struct Tex
	{
		std::shared_ptr<KdTexture> spTex = nullptr;
		Math::Vector2 pos = {};
		Math::Vector2 move = {};
		float angle = 0;
		Math::Color color = {};
	};

	std::list<Tex> m_paper{};	// 紙吹雪

	std::shared_ptr<KdTexture> m_spPaperTex;

	bool m_goalFlg = false;

	const int m_stayTime = 60;
	int m_stayCount = 0;
};