#pragma once

#include"../BaseScene/BaseScene.h"

class SceneChange;
class TitleCamera;

class TitleScene : public BaseScene
{
public :

	TitleScene() {}
	~TitleScene() {}

	void Init()	override;

private :
	void Event() override;

	// シーンチェンジ
	std::weak_ptr<SceneChange> m_wpSceneChange;

	// カメラ
	std::weak_ptr<TitleCamera> m_wpCamera;
};