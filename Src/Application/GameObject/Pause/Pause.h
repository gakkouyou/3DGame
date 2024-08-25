#pragma once

class Pause : public KdGameObject
{
public:
	Pause() {}
	~Pause()			override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void Reset()		override;

	void SetStopFlg(bool _stopFlg) { m_stopFlg = _stopFlg; }

	const bool GetPauseFlg() const { return m_pauseFlg; }

	// ボタン
	enum Select
	{
		Return,	// もどる
		Again,	// やりなおす
		Back,	// ステージから出る
		None,
	};
	// セレクトしたボタンを返す(セレクトしていなかったらNoneを返す)
	const int GetSelectButton() const { return m_select; }

private:
	struct Texture
	{
		std::shared_ptr<KdTexture>	spTex	= nullptr;
		Math::Vector3				pos		= Math::Vector3::Zero;
	};

	// もどる
	Texture m_return;

	// やりなおす
	Texture m_again;

	// ステージから出る
	Texture m_back;

	// 後ろの白い枠
	Texture m_whiteFrame;
	
	// 文字の周りの灰色の枠
	Texture m_grayFrame;

	// 選んでいるボタン
	int m_nowSelect	= Return;
	int m_select	= None;

	// pause画面かどうか
	bool m_pauseFlg = false;

	// キー制御
	enum KeyFlg
	{
		pause	= 1 << 0,
		up		= 1 << 1,
		down	= 1 << 2,
	};
	UINT m_keyFlg	= 0;

	// カーソルを動かせなくなったり、ポーズ画面を開けなくなるフラグ
	bool m_stopFlg = false;
};