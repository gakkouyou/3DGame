#include "ResultPlayer.h"
#include "../../Effect/Smoke/Smoke.h"
#include "../../../Scene/SceneManager.h"

void ResultPlayer::Update()
{
	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();

	if ((m_animation & FirstAnimationStart) && (m_animation & FirstAnimationEnd) == 0)
	{
		FirstAnimation();
	}

	if ((m_animation & SecondAnimationStart) && (m_animation & SecondAnimationEnd) == 0)
	{
		SecondAnimation();
	}

	if ((m_animation & ThirdAnimationStart) && (m_animation & ThirdAnimationEnd) == 0)
	{
		ThirdAnimation();
	}

	if ((m_animation & FourthAnimationStart) && (m_animation & FourthAnimationEnd) == 0)
	{
		FourthAnimation();
	}
}

void ResultPlayer::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void ResultPlayer::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void ResultPlayer::Init()
{
	if (!m_spModel)
	{
		// モデル
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player/player.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), false);
	}

	Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_startPos);

	m_mWorld = scaleMat * rotMat * transMat;

	// かたい地面を歩いた音
	m_wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/tileWalk.wav", false);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.06f);
		m_wpSound.lock()->Stop();
	}
}

void ResultPlayer::FirstAnimation()
{
	m_stayCount++;
	if (m_stayCount < m_stayTime) return;

	if (m_stayCount == m_stayTime)
	{
		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Run"), true);
		}
		// ドアを開ける
		m_animation |= OpenDoor;
	}

	// 手前に走る
	Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
	Math::Vector3 moveVec = { 0, 0, -1 };
	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + moveVec * m_speed);

	m_mWorld = scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (m_smokeCount % m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		m_smokeCount = 0;
		if (m_wpSound.expired() == false)
		{
			m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	m_smokeCount++;

	if (GetPos().z < -8)
	{
		m_animation |= CloseDoor;
		m_animation &= (~OpenDoor);
	}

	if (GetPos().z < m_goalPos.z)
	{
		SetPos({ GetPos().x, GetPos().y, m_goalPos.z });
		m_animation |= FirstAnimationEnd;
		m_stayCount = 0;
		// 二つ目のアニメーションをスタート
		m_animation |= SecondAnimationStart;

		// ドアをリセット
		m_animation &= (~CloseDoor);

		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		}

		// 煙を出すカウントをリセット
		m_smokeCount = 0;
	}
}

void ResultPlayer::SecondAnimation()
{
	// カウント
	m_stayCount++;
	if (m_stayCount == m_stayTime)
	{
		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Run"), true);
		}
	}
	if (m_stayCount >= m_stayTime)
	{
		// 右に走る
		Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + m_moveVec * m_speed);

		m_mWorld = scaleMat * rotMat * transMat;

		// 一定時間置きに煙を出す
		if (m_smokeCount % m_runSmokeTime == 0)
		{
			// 煙を生み出す
			std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
			smoke->Init();
			smoke->SetPos(GetPos());
			smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
			SceneManager::Instance().AddObject(smoke);
			// 煙を出すカウントをリセット
			m_smokeCount = 0;
			if (m_wpSound.expired() == false)
			{
				m_wpSound.lock()->Play();
			}
		}
		// 煙を出すカウント
		m_smokeCount++;
	}

	if (GetPos().x > m_goalPos.x)
	{
		m_animation |= SecondAnimationEnd;
		// 煙を出すカウントをリセット
		m_smokeCount = 0;
	}
}

void ResultPlayer::ThirdAnimation()
{
	// 左に走る
	Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-90.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + -m_moveVec * m_speed);

	m_mWorld = scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (m_smokeCount % m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		m_smokeCount = 0;
		if (m_wpSound.expired() == false)
		{
			m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	m_smokeCount++;

	if (GetPos().x < m_startPos.x)
	{
		m_animation |= ThirdAnimationEnd;
		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		}
		// ４つめのアニメーションを開始
		m_animation |= FourthAnimationStart;
		m_animation |= OpenDoor;
		m_stayCount = 0;

		rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(0));
		m_mWorld = scaleMat * rotMat * transMat;

		// 煙を出すカウントをリセット
		m_smokeCount = 0;
	}
}

void ResultPlayer::FourthAnimation()
{
	m_stayCount++;
	if (m_stayCount < m_stayTime) return;

	if (m_stayCount == m_stayTime)
	{
		// アニメーション変更
		if (m_spAnimator && m_spModel)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Run"), true);
		}
		m_animation &= (~OpenDoor);
	}

	// 奥に走る
	Math::Matrix scaleMat = Math::Matrix::CreateScale(8.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(0.0f));
	Math::Vector3 moveVec = { 0, 0, 1 };
	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + moveVec * m_speed);

	m_mWorld = scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (m_smokeCount % m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		m_smokeCount = 0;
		if (m_wpSound.expired() == false)
		{
			m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	m_smokeCount++;

	if (GetPos().z > m_closeDoorPosZ)
	{
		m_animation |= CloseDoor;
	}

	if (GetPos().z > m_endPosZ)
	{
		m_animation |= FourthAnimationEnd;
	}
}