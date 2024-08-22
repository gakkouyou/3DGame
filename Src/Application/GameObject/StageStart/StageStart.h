#pragma once

class StageStart : public KdGameObject
{
public:
	StageStart()				{}
	~StageStart()		override{}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	void Reset()		override;

	void SetEnd() { m_endFlg = true; }

private:
	// 画像
	std::shared_ptr<KdTexture>	m_spTex		= nullptr;

	// アルファ値
	float						m_alpha		= 0;
	// アルファ値の差
	const float					m_alphaDiff = 0.05f;

	// 透明にしていくフラグ
	bool						m_endFlg	= false;
};