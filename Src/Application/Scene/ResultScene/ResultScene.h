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
};