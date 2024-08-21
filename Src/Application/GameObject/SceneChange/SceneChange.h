#pragma once

#include "Application/Utility.h"

class SceneChange : public KdGameObject
{
public:
	SceneChange() {}
	~SceneChange()	override {}

	void Update()		override;
	void DrawSprite()	override;
	void Init()			override;

	// リセット処理
	void Reset()		override;

	// シーンを開始する
	void StartScene();
	// シーンを終了する
	void EndScene();

	// シーン遷移が終了したことを伝えるフラグ
	const bool GetFinishFlg() const { return m_finishFlg; }

private:
	// 白い丸画像
	std::shared_ptr<KdTexture> m_spCircleTex = nullptr;

	// 仮の画像
	std::shared_ptr<KdTexture> m_spTmpTex	= nullptr;

	// 画像サイズ
	float m_size			= 0;
	// サイズの増減
	const float m_addSize	= 15 ;
	// 最大サイズ
	const float m_maxSize	= 1500;
	// 最小サイズ
	const float m_minSize	= 0;

	const float m_stayTime	= 5;
	float		m_stayCnt	= 0;

	// 始まるフラグ
	bool m_startFlg = false;

	// 終わるフラグ
	bool m_endFlg = false;

	// シーン遷移が終了したことを伝えるフラグ
	bool m_finishFlg = false;
};