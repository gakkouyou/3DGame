#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class TerrainController;
class SceneChange;
class Player;
class TerrainBase;
class EventObjectController;
class FinalGoal;

class StageSelectScene : public BaseScene
{
public:

	StageSelectScene() {}
	~StageSelectScene() {}

	void Init()	override;

	const bool GetDebug() const override { return m_debugFlg; }

	void SetFinalGoal(const std::weak_ptr<FinalGoal>& _wpFinalGoal) { m_wpFinalGoal = _wpFinalGoal; }

private:

	void Event() override;

	// ステージセレクトシーンがスタートしたときの処理
	void StartScene();

	// ステージを初クリアしたときの処理
	void FirstClearProcess();

	// 坂の演出
	void SlopeEffectProcess();
	// 最終ゴールの演出
	void FinalGoalEffectProcess();

	// TPSカメラ
	std::weak_ptr<TPSCamera> m_wpCamera;

	// TerrainController
	std::weak_ptr<TerrainController> m_wpTerrainController;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// EventObjectController
	std::weak_ptr<EventObjectController> m_wpEventObjectController;

	// 最終ゴール
	std::weak_ptr<FinalGoal> m_wpFinalGoal;

	// シーンをリセットした時のフラグ
	bool m_resetFlg = false;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;

	// ステージ初クリアの際に演出を行うオブジェクトを保持
	std::weak_ptr<TerrainBase> m_wpFirstClearObject;
	// 坂の演出中のフラグ
	bool m_slopeEffectFlg = false;
	// 最終ゴールの演出中のフラグ
	bool m_finalGoalEffectFlg = false;
	// 初クリアかどうかのフラグ
	bool m_firstClearFlg = false;
};
