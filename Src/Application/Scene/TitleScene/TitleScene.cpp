#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/SceneChange/SceneChange.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!m_wpSceneChange.expired())
		{
			m_wpSceneChange.lock()->EndScene();
		}
	}

	if (!m_wpSceneChange.expired())
	{
		if (m_wpSceneChange.lock()->GetFinishFlg())
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::StageSelect
			);
		}
	}
}

void TitleScene::Init()
{
	// シーンチェンジ
	std::shared_ptr<SceneChange> sceneChange = std::make_shared<SceneChange>();
	sceneChange->Init();
	AddObject(sceneChange);
	// 保持
	m_wpSceneChange = sceneChange;
}
