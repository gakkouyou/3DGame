#pragma once

#include"../BaseScene/BaseScene.h"

class SceneChange;
class ResultPlayer;
class ResultBackGround;

class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene() {}

	void Init()	override;

	const bool GetDebug() const override { return true; }

private:
	void Event() override;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// プレイヤー
	std::weak_ptr<ResultPlayer> m_wpPlayer;

	// 背景
	std::weak_ptr<ResultBackGround> m_wpBackGround;

	// シーンが開始した瞬間がわかるフラグ
	bool m_sceneStartFlg = false;

	float m_ambientLight = 0.6f;
	float m_addAmbientLight = 0.0015f;
};