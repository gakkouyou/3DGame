﻿#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class Player;
class SceneChange;
class Goal;
class Result;
class Pause;
class GameUI;
class EnemyController;

class GameScene : public BaseScene
{
public :

	GameScene() {}
	~GameScene() {}

	void Init()	override;

	const bool GetDebug() const override { return m_debugFlg; }

private:

	void Event() override;

	// 他のシーンから変わって、ゲームシーンが始まった時の処理
	void StartGameScene();
	// シーンを移り変わらずに、ゲームシーンを終了する _stayCnt:終了までの時間
	void GameEnd(int _stayCnt);
	// シーンを移り変わらずに、ゲームシーンを再開する
	void GameSceneReStart();

	// ステージの色々をリセット
	void ResetCSV();

	// TPSカメラ
	std::weak_ptr<TPSCamera> m_wpCamera;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// リザルト
	std::weak_ptr<Result> m_wpResult;

	// ポーズ画面
	std::weak_ptr<Pause> m_wpPause;

	// 敵のエディタ
	std::weak_ptr<EnemyController> m_wpEnemyController;

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

	// ポーズ画面かどうか
	bool m_pauseFlg		= false;
	bool m_oldPauseFlg	= false;

	std::weak_ptr<KdSoundInstance> m_bgm;
	float m_vol = 0.03f;
	// bgmフェードアウトさせる時の数値
	const float m_sumVol = 0.002f;
};
