#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class TerrainController;
class Player;
class SceneChange;

class GameScene : public BaseScene
{
public :

	GameScene() {}
	~GameScene() {}

	void Init()	override;

	const bool GetDebug() const override { return m_debugFlg; }

private:

	void Event() override;

	// TPSカメラ
	std::weak_ptr<TPSCamera> m_wpCamera;

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
