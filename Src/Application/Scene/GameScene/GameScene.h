#pragma once

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

	// シーンが始まった時の処理用のフラグ
	bool m_sceneStartFlg = false;

	// シーンをリセットした時のフラグ
	bool m_resetFlg = false;

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;

	// ゴールフラグ
	bool m_goalFlg = false;

	// ミスしたときのリスタートするまでの時間
	const int m_restartTime = 60;

	// フェードインを始めるまでの時間
	const int m_fadeInTime = 5;

	// 今のステージ
	int m_nowStage	= 0;

	// ポーズ画面かどうか
	bool m_pauseFlg		= false;
	bool m_oldPauseFlg	= false;

	std::weak_ptr<KdSoundInstance> m_bgm;
	float m_vol = 0.03f;
	// bgmフェードアウトさせる時の数値
	const float m_sumVol = 0.002f;

	// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	(GameScene&)	{}
		virtual void Update	(GameScene&)	{}
		virtual void Exit	(GameScene&)	{}
	};

	// シーン開始
	class SceneStart : public StateBase
	{
	public:
		~SceneStart()	override {}

		void Enter	(GameScene& _owner)	override;
		void Update	(GameScene& _owner)	override;
		void Exit	(GameScene& _owner)	override;
	};

	// ゲームシーンをリスタートする前の終了処理
	class GameEnd : public StateBase
	{
	public:
		~GameEnd()	override {}

		void Enter	(GameScene& _owner)	override;
		void Update	(GameScene& _owner)	override;
		void Exit	(GameScene& _owner)	override;
	};

	// ゲームシーンをリスタートする時の処理
	class GameRestart : public StateBase
	{
	public:
		~GameRestart()	override {}

		void Enter	(GameScene& _owner)	override;
		void Update	(GameScene& _owner)	override;
		void Exit	(GameScene& _owner)	override;
	};

	// ゲーム
	class Game : public StateBase
	{
	public:
		~Game()	override {}

		void Enter(GameScene& _owner)	override;
		void Update(GameScene& _owner)	override;
		void Exit(GameScene& _owner)	override;
	};

	// ミス
	class Miss : public StateBase
	{
	public:
		~Miss()	override {}

		void Enter(GameScene& _owner)	override;
		void Update(GameScene& _owner)	override;
		void Exit(GameScene& _owner)	override;
	};

	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};
