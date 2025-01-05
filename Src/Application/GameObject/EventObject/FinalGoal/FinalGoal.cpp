#include "FinalGoal.h"
#include "../../Effect/Smoke/Smoke.h"
#include "../../../Scene/SceneManager.h"

void FinalGoal::Update()
{
	if (m_param.modelNum == 3 && m_firstClear == FirstClear::EndProcess)
	{
		// 時計の音を鳴らす
		if (!m_soundFlg)
		{
			if (m_wpSound.expired() == false)
			{
				m_wpSound.lock()->Play(true);
			}
			m_soundFlg = true;
		}
		// 座標を入れる
		m_wpSound.lock()->SetPos(m_pos);
	}
}

void FinalGoal::GenerateDepthMapFromLight()
{
	if (m_spOldModel && m_firstClearFlg)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOldModel, m_mWorld);
	}

	Math::Matrix mat;
	if (m_firstClearFlg)
	{
		mat = m_newMat;
	}
	else
	{
		mat = m_mWorld;
	}
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}
}

void FinalGoal::DrawLit()
{
	if (m_spOldModel && m_firstClearFlg)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spOldModel, m_mWorld);
	}

	Math::Matrix mat;
	if (m_firstClearFlg)
	{
		mat = m_newMat;
	}
	else
	{
		mat = m_mWorld;
	}
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
	}
}

void FinalGoal::Init()
{
	EventObjectBase::Init();

	// コライダー
	m_pCollider = std::make_unique<KdCollider>();

	m_objectType = ObjectType::FinalGoal;

	m_wpSound = KdAudioManager::Instance().Play3D("Asset/Sounds/SE/alarm.wav", m_pos, true);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.3f);
		m_wpSound.lock()->Stop();
	}
	m_soundFlg = false;

	// 新しいモデルの拡縮を０にしておく
	m_newMat = Math::Matrix::CreateScale(0.0f);
}

void FinalGoal::SetParam(const Param& _param)
{
	m_param = _param;

	m_pos = _param.basePos;

	m_setParamFlg = false;

	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_defaultScale);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
	m_mWorld = scaleMat * rotMat;
	m_mWorld.Translation(m_pos);
}

void FinalGoal::FirstClearProcess()
{
	m_firstClear = FirstClear::StartProcess;

	// 一番最初だけ煙を出す
	if (m_smokeFlg == false)
	{
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(m_newPos);
		smoke->SetSmokeType(Smoke::SmokeType::AppearanceSmoke);
		SceneManager::Instance().AddObject(smoke);

		// 音
		std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sounds/SE/appearance.wav");
		se->SetVolume(0.06f);
	}

	m_smokeFlg = true;

	if (m_sumFlg == false)
	{
		// 大きくしていく
		m_scale += m_addScale;
		// 加算値も大きくしていく
		m_addScale += m_addAddScale;
		// 上限を超えたら少し小さくしていく
		if (m_scale > m_maxScale)
		{
			m_scale = m_maxScale;
			m_sumFlg = true;
		}
	}
	else
	{
		// 小さくしていく
		m_scale -= m_sumScale;
		// 下限まで行ったら少し待機して終了
		if (m_scale < m_defaultScale)
		{
			m_scale = m_defaultScale;
			m_stayCount++;
			if (m_stayCount > m_stayTime)
			{
				m_firstClear = FirstClear::EndProcess;
			}
		}
	}

	// 行列確定
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_newPos);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

	//m_mWorld = scaleMat * rotMat * transMat;
	m_newMat = scaleMat * rotMat * transMat;
}

void FinalGoal::SetFirstClearFlg(bool _firstClearFlg)
{
	m_firstClearFlg = _firstClearFlg;

	switch (m_param.modelNum)
	{
		// ステージ１をクリアしたときのモデル
	case 1:
		m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Body/body.gltf");
		break;

		// ステージ２をクリアしたときのモデル
	case 2:
		if (m_firstClearFlg)
		{
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Needle/needle.gltf");
		}
		else
		{
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/BodyNeedle/bodyNeedle.gltf");
		}
		break;

		// ステージ３をクリアしたときのモデル
	case 3:
		if (m_firstClearFlg)
		{
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Bell/bell.gltf");
		}
		else
		{
			m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/clock.gltf");
			m_pCollider->RegisterCollisionShape("FinalGoal", m_spModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
		}
		break;
	}

	if (_firstClearFlg == false) return;

	// 一つ前のモデルをロード
	switch (m_param.modelNum)
	{
	case 2:
		m_spOldModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/Body/body.gltf");
		break;

	case 3:
		m_spOldModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/EventObject/Clock/BodyNeedle/bodyNeedle.gltf");
		m_pCollider->RegisterCollisionShape("FinalGoal", m_spOldModel, KdCollider::TypeEvent | KdCollider::TypeDebug);
		break;
	}
	// モデルが存在するなら、新しいモデルを置くべき位置を古いモデルから取得
	if (m_spOldModel)
	{
		m_newPos = (m_spOldModel->FindNode("NewPoint")->m_worldTransform * m_mWorld).Translation();
	}
	// モデルが存在しないなら、指定された位置にする
	else
	{
		m_newPos = m_pos;
	}
}
