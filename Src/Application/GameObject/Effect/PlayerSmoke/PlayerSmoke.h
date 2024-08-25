#pragma once

class PlayerSmoke : public KdGameObject
{
public:
	PlayerSmoke() {}
	~PlayerSmoke()	override {}

	void Update()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetPauseFlg(bool _pauseFlg)	override { m_pauseFlg = _pauseFlg; }

private:
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	float m_scale = 0;

	// ポーズ画面かどうか
	bool m_pauseFlg = false;
};