#include "ResultPlayer.h"
#include "../../Effect/Smoke/Smoke.h"
#include "../../../Scene/SceneManager.h"

void ResultPlayer::Update()
{
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}
	// アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
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
	DataLoad();

	m_scaleMat = Math::Matrix::CreateScale(m_scale);

	if (!m_spModel)
	{
		// モデル
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player/player.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), false);
	}

	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_startPos);

	m_mWorld = m_scaleMat * rotMat * transMat;

	// かたい地面を歩いた音
	m_wpSound = KdAudioManager::Instance().Play("Asset/Sounds/SE/tileWalk.wav", false);
	if (!m_wpSound.expired())
	{
		m_wpSound.lock()->SetVolume(0.06f);
		m_wpSound.lock()->Stop();
	}

	// 最初は待機状態
	m_nowAction = std::make_shared<Stay>();
}

void ResultPlayer::DataLoad()
{
	// JSONファイルを読み込む
	std::ifstream file(m_path.data());
	if (!file.is_open()) return;

	nlohmann::json data;
	file >> data;

	// JSONデータを格納していく
	m_moveSpeed			= data["ResultPlayer"]["m_moveSpeed"];			// 移動速度
	m_startPos.x		= data["ResultPlayer"]["m_startPos.x"];			// 初期座標
	m_startPos.y		= data["ResultPlayer"]["m_startPos.y"];
	m_startPos.z		= data["ResultPlayer"]["m_startPos.z"];
	m_goalPos.x			= data["ResultPlayer"]["m_goalPos.x"];			// ゴール座標
	m_goalPos.y			= data["ResultPlayer"]["m_goalPos.y"];
	m_goalPos.z			= data["ResultPlayer"]["m_goalPos.z"];
	m_stayTime			= data["ResultPlayer"]["m_stayTime"];			// 待機時間
	m_closeDoorPosZ		= data["ResultPlayer"]["m_closeDoorPosZ"];		// ドアを閉める座標
	m_animationEndPosZ	= data["ResultPlayer"]["m_animationEndPosZ"];	// アニメーションが終了する座標
	m_runSmokeTime		= data["ResultPlayer"]["m_runSmokeTime"];		// 煙の頻度
	m_scale				= data["ResultPlayer"]["m_scale"];				// 拡縮
}

void ResultPlayer::ChangeActionState(std::shared_ptr<StateBase> _nextState)
{
	if (m_nowAction) m_nowAction->Exit(*this);
	m_nowAction = _nextState;
	if(m_nowAction) m_nowAction->Enter(*this);
}

void ResultPlayer::FirstAnimation::Update(ResultPlayer& _owner)
{
	// 少し待機
	_owner.m_stayCount++;
	if (_owner.m_stayCount < _owner.m_stayTime) return;

	// 待機終了した瞬間のみ処理
	if (_owner.m_stayCount == _owner.m_stayTime)
	{
		// アニメーション変更
		if (_owner.m_spAnimator && _owner.m_spModel)
		{
			_owner.m_spAnimator->SetAnimation(_owner.m_spModel->GetAnimation("Run"), true);
		}
		// ドアを開ける
		_owner.m_animation |= Animation::OpenDoor;
	}

	// 手前に走る
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_owner.GetPos() + Math::Vector3::Forward * _owner.m_moveSpeed);

	_owner.m_mWorld = _owner.m_scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (_owner.m_smokeCount % _owner.m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(_owner.GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		_owner.m_smokeCount = 0;
		// 足音を鳴らす
		if (_owner.m_wpSound.expired() == false)
		{
			_owner.m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	_owner.m_smokeCount++;

	// 少し前に出たらドアを閉める
	if (_owner.GetPos().z < -8)
	{
		_owner.m_animation |= CloseDoor;
		_owner.m_animation &= (~OpenDoor);
	}

	// ゴールのZ座標に着いたら終了
	if (_owner.GetPos().z < _owner.m_goalPos.z)
	{
		_owner.ChangeActionState(std::make_shared<SecondAnimation>());
	}
}

void ResultPlayer::FirstAnimation::Exit(ResultPlayer& _owner)
{
	// 座標をゴールの座標にする
	_owner.SetPos({ _owner.GetPos().x, _owner.GetPos().y, _owner.m_goalPos.z });
	// 待機カウントリセット
	_owner.m_stayCount = 0;

	// ドアをリセット
	_owner.m_animation &= (~CloseDoor);

	// アニメーション変更
	if (_owner.m_spAnimator && _owner.m_spModel)
	{
		_owner.m_spAnimator->SetAnimation(_owner.m_spModel->GetAnimation("Idle"), true);
	}

	// 煙を出すカウントをリセット
	_owner.m_smokeCount = 0;
}

void ResultPlayer::SecondAnimation::Update(ResultPlayer& _owner)
{
	// 少し待機
	_owner.m_stayCount++;
	if (_owner.m_stayCount < _owner.m_stayTime) return;

	if (_owner.m_stayCount == _owner.m_stayTime)
	{
		// アニメーション変更
		if (_owner.m_spAnimator && _owner.m_spModel)
		{
			_owner.m_spAnimator->SetAnimation(_owner.m_spModel->GetAnimation("Run"), true);
		}
	}
	// 右に走る
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_owner.GetPos() + Math::Vector3::Right * _owner.m_moveSpeed);

	_owner.m_mWorld = _owner.m_scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (_owner.m_smokeCount % _owner.m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(_owner.GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		_owner.m_smokeCount = 0;
		// 足音を鳴らす
		if (_owner.m_wpSound.expired() == false)
		{
			_owner.m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	_owner.m_smokeCount++;

	// ゴールについたら一旦終了
	if (_owner.GetPos().x > _owner.m_goalPos.x)
	{
		_owner.ChangeActionState(std::make_shared<Stay>());
	}
}

void ResultPlayer::SecondAnimation::Exit(ResultPlayer& _owner)
{
	_owner.m_animation |= MorningAnimationEnd;
	// 煙を出すカウントをリセット
	_owner.m_smokeCount = 0;
}

void ResultPlayer::ThirdAnimation::Update(ResultPlayer& _owner)
{
	// 左に走る
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-90.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_owner.GetPos() + Math::Vector3::Left * _owner.m_moveSpeed);

	_owner.m_mWorld = _owner.m_scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (_owner.m_smokeCount % _owner.m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(_owner.GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		_owner.m_smokeCount = 0;
		if (_owner.m_wpSound.expired() == false)
		{
			_owner.m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	_owner.m_smokeCount++;

	// ゴールに着いたら終了
	if (_owner.GetPos().x < _owner.m_startPos.x)
	{
		_owner.ChangeActionState(std::make_shared<FourthAnimation>());
	}
}

void ResultPlayer::ThirdAnimation::Exit(ResultPlayer& _owner)
{
	// アニメーション変更
	if (_owner.m_spAnimator && _owner.m_spModel)
	{
		_owner.m_spAnimator->SetAnimation(_owner.m_spModel->GetAnimation("Idle"), true);
	}
	// ドアを空ける
	_owner.m_animation |= OpenDoor;
	// 待機カウントリセット
	_owner.m_stayCount = 0;

	// 家の方向を向かす
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(0);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_owner.GetPos());
	_owner.m_mWorld = _owner.m_scaleMat * rotMat * transMat;

	// 煙を出すカウントをリセット
	_owner.m_smokeCount = 0;
}

void ResultPlayer::FourthAnimation::Update(ResultPlayer& _owner)
{
	// 少し待機
	_owner.m_stayCount++;
	if (_owner.m_stayCount < _owner.m_stayTime) return;

	if (_owner.m_stayCount == _owner.m_stayTime)
	{
		// アニメーション変更
		if (_owner.m_spAnimator && _owner.m_spModel)
		{
			_owner.m_spAnimator->SetAnimation(_owner.m_spModel->GetAnimation("Run"), true);
		}
		_owner.m_animation &= (~OpenDoor);
	}

	// 奥に走る
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(0.0f));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_owner.GetPos() + Math::Vector3::Backward * _owner.m_moveSpeed);

	_owner.m_mWorld = _owner.m_scaleMat * rotMat * transMat;

	// 一定時間置きに煙を出す
	if (_owner.m_smokeCount % _owner.m_runSmokeTime == 0)
	{
		// 煙を生み出す
		std::shared_ptr<Smoke> smoke = std::make_shared<Smoke>();
		smoke->Init();
		smoke->SetPos(_owner.GetPos());
		smoke->SetSmokeType(Smoke::SmokeType::WalkSmoke);
		SceneManager::Instance().AddObject(smoke);
		// 煙を出すカウントをリセット
		_owner.m_smokeCount = 0;
		// 音を鳴らす
		if (_owner.m_wpSound.expired() == false)
		{
			_owner.m_wpSound.lock()->Play();
		}
	}
	// 煙を出すカウント
	_owner.m_smokeCount++;

	// ドアを閉める
	if (_owner.GetPos().z > _owner.m_closeDoorPosZ)
	{
		_owner.m_animation |= CloseDoor;
	}

	// ゴールに着いたら終了
	if (_owner.GetPos().z > _owner.m_animationEndPosZ)
	{
		_owner.ChangeActionState(nullptr);
	}
}

void ResultPlayer::FourthAnimation::Exit(ResultPlayer& _owner)
{
	_owner.m_animation |= Animation::EveningAnimationEnd;
}

void ResultPlayer::Stay::Update(ResultPlayer& _owner)
{
	// 朝のアニメーションが終了しておらず、朝のアニメーションを開始する状態なら朝のアニメーションを開始する
	if (_owner.m_morningAnimationStart == true && !(_owner.m_animation & Animation::MorningAnimationEnd))
	{
		_owner.ChangeActionState(std::make_shared<FirstAnimation>());
	}
	// 夕方のアニメーションが終了しておらず、夕方のアニメーションを開始する状態なら夕方のアニメーションを開始する
	if (_owner.m_eveningAnimationStart == true && !(_owner.m_animation & Animation::EveningAnimationEnd))
	{
		_owner.ChangeActionState(std::make_shared<ThirdAnimation>());
	}
}
