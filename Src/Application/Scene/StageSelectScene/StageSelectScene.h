#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class TerrainController;
class SceneChange;
class Player;

class StageSelectScene : public BaseScene
{
public:

	StageSelectScene() {}
	~StageSelectScene() {}

	void Init()	override;

	const bool GetDebug() const override { return m_debugFlg; }

private:

	void Event() override;

	// ステージセレクトシーンがスタートしたときの処理
	void StartScene();

	// ステージを初クリアしたときの処理
	void FirstClearProcess();

	// TPSカメラ
	std::weak_ptr<TPSCamera> m_wpCamera;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// シーンをリセットした時のフラグ
	bool m_resetFlg = false;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;
};
