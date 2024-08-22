#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class Player;
class SceneChange;
class Goal;
class Result;
class StageStart;

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

	// ゴール
	std::weak_ptr<Goal> m_wpGoal;

	// リザルト
	std::weak_ptr<Result> m_wpResult;

	// "Stage Start"
	std::weak_ptr<StageStart> m_wpStart;

	// シーンをリセットした時のフラグ
	bool m_resetFlg = false;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;

	// ゴールフラグ
	bool m_goalFlg = false;

	// 今のステージ
	int m_nowStage	= 0;

	// ステージの状況
	std::vector<int> m_stageInfoList;

	void CSVLoader();
	void CSVWriter();
};
