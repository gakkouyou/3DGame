#pragma once

#include"../BaseScene/BaseScene.h"

class SceneChange;
class ResultPlayer;
class ResultBackGround;
class House;

class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene() {}

	void Init()	override;

private:
	void Event() override;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// プレイヤー
	std::weak_ptr<ResultPlayer> m_wpPlayer;

	// 背景
	std::weak_ptr<ResultBackGround> m_wpBackGround;

	// 家
	std::weak_ptr<House> m_wpHouse;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	struct Sound
	{
		bool flg = false;
		std::weak_ptr<KdSoundInstance> sound;
		float vol = 0;
		float sumVol = 0;
	};
	
	// BGM
	Sound m_bgm;
	// すずめの鳴き声
	Sound m_morning;
	// からすの鳴き声
	Sound m_evening;

// ステートパターン
private:
	class StateBase
	{
	public:
		virtual ~StateBase() {}

		virtual void Enter	(ResultScene&)	{}
		virtual void Update	(ResultScene&)	{}
		virtual void Exit	(ResultScene&)	{}
	};

	// シーン開始
	class SceneStart : public StateBase
	{
	public:
		~SceneStart()	override {}

		void Enter	(ResultScene& _owner)	override;
		void Update	(ResultScene& _owner)	override;
		void Exit	(ResultScene& _owner)	override;
	};

	// 朝
	class Morning : public StateBase
	{
	public:
		~Morning()	override {}

		void Update	(ResultScene& _owner)	override;
		void Exit	(ResultScene& _owner)	override;
	};

	// 夕方
	class Evening : public StateBase
	{
	public:
		~Evening()	override {}

		void Enter	(ResultScene& _owner)	override;
		void Update	(ResultScene& _owner)	override;
	};

	// 夜
	class Night : public StateBase
	{
	public:
		~Night()	override {}

		void Enter(ResultScene& _owner)	override;
		void Update(ResultScene& _owner)	override;
	};
	void ChangeActionState(std::shared_ptr<StateBase> _nextState);
	std::shared_ptr<StateBase> m_nowAction = nullptr;
};