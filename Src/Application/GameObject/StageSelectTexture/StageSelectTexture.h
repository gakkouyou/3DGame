#pragma once

class StageSelectTexture : public KdGameObject
{
public:
	StageSelectTexture()			{}
	~StageSelectTexture()	override { CSVWriter(); }

	void Update()			override;
	void DrawSprite()		override;
	void Init()				override;

	const int GetNowStage() const { return m_nowStage; }
	const int GetMaxStage()	const { return StageNum::Max - 1; }

	// ステージの状況
	enum StageInfo
	{
		ImPossible,	// チャレンジ不可能
		Possible,	// チャレンジ可能
		Clear,		// クリア
	};
	const int GetStageInfo() const { return m_stageInfoList[m_nowStage]; }

	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

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

	int m_stageInfoList[StageNum::Max]	= {};

	// 止めるフラグ
	bool m_stopFlg		= false;

	// CSV読み込み
	void CSVLoader();
	// CSV書き込み
	void CSVWriter();
};