#include "ResultBackGround.h"
#include "../../../Scene/SceneManager.h"

void ResultBackGround::Update()
{
	// 青からオレンジにしていく
	if (m_orangeAnimationFlg && !m_orangeAnimationEndFlg)
	{
		m_color = Math::Vector3::Lerp(m_startColor, m_goalColor, m_progress);
		m_progress += m_speed;

		if (m_progress > 1)
		{
			m_progress = 0.0f;
			// 青からオレンジ終了
			m_orangeAnimationEndFlg = true;
		}

		// 環境光も弱めていく
		// 半分まで
		m_ambientLight -= m_addAmbientLight;
		if (m_ambientLight < m_halfAmbientLight)
		{
			m_ambientLight = m_halfAmbientLight;
		}
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight, 1.0f });
	}

	// オレンジから黒にしていく
	if (m_blackAnimationFlg)
	{
		//m_color = Math::Vector3::Lerp(m_goalColor, { 0, 0, 0 }, m_progress);
		//m_progress += m_speed;

		//if (m_progress > 1)
		//{
		//	m_progress = 1.0f;
		//}

		// 夕方
		m_alpha -= m_speed;
		if (m_alpha < 0)
		{
			m_alpha = 0;
		}

		// 環境光も弱めていく
		// 0まで 
		m_ambientLight -= m_addAmbientLight;
		if (m_ambientLight < 0)
		{
			m_ambientLight = 0;
			// 音を全て止める
			KdAudioManager::Instance().StopAllSound();
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
		}
		KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ m_ambientLight, m_ambientLight, m_ambientLight, 1.0f });
	}
}

void ResultBackGround::DrawUnLit()
{
	if (m_spNightModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		Math::Matrix mat = m_mWorld;
		Math::Vector3 pos = GetPos();
		pos.z += 0.1f;
		mat.Translation(pos);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spNightModel, mat);

		KdShaderManager::Instance().UndoRasterizerState();
	}

	if (m_spModel)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);

		Math::Color color = { 1, 1, 1, m_alpha };

		KdShaderManager::Instance().m_StandardShader.SetColorEnable(true, m_color);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, color);

		KdShaderManager::Instance().UndoRasterizerState();
	}
}

void ResultBackGround::Init()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/BackGround/ResultBackGround/resultBackGround.gltf");
	m_spNightModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Title/BackGround/backGround.gltf");

	Math::Matrix scaleMat = Math::Matrix::CreateScale(9.5f);

	m_mWorld = scaleMat;

	m_mWorld.Translation({ 0, -5, 20 });

	m_startColor.x = 160 / 255.0f;
	m_startColor.y = 216 / 255.0f;
	m_startColor.z = 239 / 255.0f;

	m_color = m_startColor;

	m_goalColor.x = 238 / 255.0f;
	m_goalColor.y = 120 / 255.0f;
	m_goalColor.z = 0;

	m_speed = 1.0f / m_frame;

	m_halfAmbientLight = m_ambientLight / 2;
	// 引いていく環境光の値
	m_addAmbientLight = m_ambientLight / (m_frame * 2);
}
