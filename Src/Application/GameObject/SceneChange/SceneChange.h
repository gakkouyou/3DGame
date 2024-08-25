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
	void StartScene(int _stayTime = 0);
	// シーンを終了する
	void EndScene(int _stayTime = 0);

	// シーン遷移が終了したことを伝えるフラグ
	const bool GetFinishFlg() const { return m_finishFlg; }

	// シーン遷移が行われているかどうかのフラグ
	const bool GetNowSceneChange() const { return m_startFlg || m_endFlg; }

private:
	// 白い丸画像
	std::shared_ptr<KdTexture> m_spCircleTex = nullptr;

	// 仮の画像
	std::shared_ptr<KdTexture> m_spTmpTex	= nullptr;

	// 画像サイズ
	float m_size			= 0;
	// サイズの増える数値
	const float m_addSize	= 30;
	// サイズの減る数値
	const float m_subSize	= 15;
	// 最大サイズ
	const float m_maxSize	= 1500;
	// 最小サイズ
	const float m_minSize	= 0;

	float		m_stayTime	= 0;
	float		m_stayCnt	= 0;

	// 始まるフラグ
	bool m_startFlg = false;

	// 終わるフラグ
	bool m_endFlg = false;

	// シーン遷移が終了したことを伝えるフラグ
	bool m_finishFlg = false;
};