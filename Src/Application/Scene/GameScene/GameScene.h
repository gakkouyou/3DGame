#pragma once

#include"../BaseScene/BaseScene.h"

class TPSCamera;
class ObjectController;

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

	// デバッグ用
	bool m_debugFlg = false;
	bool m_debugKeyFlg = false;
};
