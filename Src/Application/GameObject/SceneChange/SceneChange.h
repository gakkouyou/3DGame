#pragma once

class SceneChange : public KdGameObject
{
public:
	SceneChange() {}
	~SceneChange()	override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// シーンを開始する
	void StartScene();
	// シーンを終了する
	void EndScene();

private:
	// 白い丸画像
	std::shared_ptr<KdTexture> m_spCircleTex = nullptr;

	// 仮の画像
	std::shared_ptr<KdTexture> m_spTmpTex	= nullptr;

	// 画像サイズ
	Math::Vector2 m_size					= Math::Vector2::Zero;

	// 始まるフラグ
	bool m_startFlg = false;

	// 終わるフラグ
	bool m_endFlg = false;
};