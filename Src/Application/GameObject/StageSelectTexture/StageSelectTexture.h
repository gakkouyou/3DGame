#pragma once

class StageSelectTexture : public KdGameObject
{
public:
	StageSelectTexture()			{}
	~StageSelectTexture()	override{}

	void Update()			override;
	void DrawSprite()		override;
	void Init()				override;

	const int GetNowStage() const { return m_nowStage; }
	const int GetMaxStage()	const { return StageNum::Max - 1; }

private:
	enum StageNum
	{
		Stage1,
		Stage2,
		Stage3,
		Max
	};

	// 画像
	std::shared_ptr<KdTexture> m_spTex[StageNum::Max];

	// 今のステージ
	int m_nowStage = Stage1;

	// ステージセレクトボタンのキーフラグ
	bool m_rightKeyFlg	= false;
	bool m_leftKeyFlg	= false;
};