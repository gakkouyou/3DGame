#pragma once

#include"../BaseScene/BaseScene.h"

class StageSelectCamera;
class TerrainController;
class StageSelectPlayer;
class SceneChange;
class StageSelectTexture;
class StageSelectUI;

class StageSelectScene : public BaseScene
{
public:

	StageSelectScene() {}
	~StageSelectScene() {}

	void Init()	override;

	const bool GetDebug() const override { return m_debugFlg; }

private:

	void Event() override;

	// TPSカメラ
	std::weak_ptr<StageSelectCamera> m_wpCamera;

	// プレイヤー
	std::weak_ptr<StageSelectPlayer> m_wpPlayer;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// ステージセレクトの画像クラス
	std::weak_ptr<StageSelectTexture> m_wpStageSelectTexture;

	// UI
	std::weak_ptr<StageSelectUI> m_wpUI;

	// シーンをリセットした時のフラグ
	bool m_resetFlg = false;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;
};
