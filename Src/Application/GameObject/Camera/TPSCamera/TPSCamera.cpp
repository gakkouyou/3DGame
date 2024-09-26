#include "TPSCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"

void TPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	const std::shared_ptr<const Player>	spTarget	= m_wpTarget.lock();
	if (!spTarget) return;
	// ターゲットの座標
	Math::Vector3	targetPos = spTarget->GetPos();
	// ターゲットの座標行列
	Math::Matrix	targetMat = Math::Matrix::CreateTranslation(targetPos);


	if (SceneManager::Instance().GetDebug())
	{
		Math::Matrix debugMat = Math::Matrix::Identity;
		m_moveVec = Math::Vector3::Zero;

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			if (!m_shiftFlg)
			{
				m_shiftFlg = true;
				SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
			}
			else
			{
				UpdateRotateByMouse();
			}
		}
		else
		{
			m_shiftFlg = false;
		}

		// WASDで移動
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_moveVec.z += 1.0f;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_moveVec.x -= 1.0f;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_moveVec.z -= 1.0f;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_moveVec.x += 1.0f;
		}

		// カメラの向きで移動方向を補正
		Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_DegAng.y), 0, DirectX::XMConvertToRadians(m_DegAng.z));
		m_moveVec = m_moveVec.TransformNormal(m_moveVec, rotMat);
		m_moveVec.Normalize();
		m_moveVec.y = 0;

		// スペースで上に行く
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_moveVec.y += 1.0f;
		}
		// controlで下に行く
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			m_moveVec.y -= 1.0f;
		}


		m_debugPos += m_moveVec * m_moveSpeed;

		debugMat = Math::Matrix::CreateTranslation(m_debugPos);

		// カメラの回転
		m_mRotation = GetRotationMatrix();

		// ローカル行列変更
		Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 3.0f, -5.0f);

		m_mWorld = m_mRotation * transMat * targetMat * debugMat;
	}
	else
	{
		if (m_pauseFlg == false)
		{
			m_debugPos = Math::Vector3::Zero;
			m_DegAng = Math::Vector3::Zero;

			// 追尾すべきかすべきでないか
			bool trackingFlg = spTarget->IsCameraTracking();

			// 追尾すべきでない場合
			if (trackingFlg == false)
			{
				const float up = 2.0f;
				const float down = 0.0f;
				// 一定以上上に上がった時追尾する
				if (targetPos.y - m_oldPlayerPos.y >= up)
				{
					targetPos.y = m_oldPlayerPos.y + (targetPos.y - m_oldPlayerPos.y) - up;
				}
				// 一定以上下に下がった時追尾する
				else if (targetPos.y - m_oldPlayerPos.y <= -down)
				{
					targetPos.y = m_oldPlayerPos.y + (targetPos.y - m_oldPlayerPos.y) + down;
				}
				// 追尾しない
				else
				{
					targetPos.y = m_oldPlayerPos.y;
				}
			}

			// 滑らかに動くようにする
			targetMat.Translation(Math::Vector3::Lerp(m_mWorld.Translation() - m_mLocalPos.Translation(), targetPos, 0.1));

			// 行列確定
			m_mWorld = m_mLocalPos * targetMat;

			// 追尾している時のみプレイヤーの座標を保持しておく
			if (trackingFlg == true)
			{
				m_oldPlayerPos = targetPos;
			}
		}
	}

	if (m_goalFlg)
	{
		GoalProcess();
	}

	// ポーズ画面の処理
	if (m_oldPauseFlg != m_pauseFlg)
	{
		if (m_pauseFlg == true)
		{
			m_spCamera->SetFocus(0, 0, m_pauseFocus);
		}
		else
		{
			m_spCamera->SetFocus(0, 0, m_defaultFocus);
		}
	}
	m_oldPauseFlg = m_pauseFlg;

	CameraBase::Update();

	KdAudioManager::Instance().SetListnerMatrix(m_mWorld);
}

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 4.5f, -6.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(20.0f));
	m_mLocalPos = rotMat * transMat;

	if (m_wpTarget.expired() == false)
	{
		m_mWorld = m_mLocalPos * Math::Matrix::CreateTranslation(m_wpTarget.lock()->GetPos());
	}

	m_goalProcess.moveFrame = 150;
	m_goalProcess.startPos	= { 0, 4, -9 };
	m_goalProcess.endPos	= { 0, 6, -20 };
	m_goalProcess.nowPos	= m_goalProcess.startPos;
	m_goalProcess.moveVec	= m_goalProcess.endPos - m_goalProcess.startPos;
	m_goalProcess.moveVec.Normalize();
	m_goalProcess.speed		= (m_goalProcess.endPos - m_goalProcess.startPos).Length() / m_goalProcess.moveFrame;
	m_goalProcess.startDegAng	= { 0, 0, 0 };
	m_goalProcess.endDegAng		= { 20, 0, 0 };
	m_goalProcess.nowDegAng		= m_goalProcess.startDegAng;
	m_goalProcess.moveDegAng.x	= (m_goalProcess.endDegAng.x - m_goalProcess.startDegAng.x) / m_goalProcess.moveFrame;
	m_goalProcess.stayFrame		= 60;
}

void TPSCamera::Reset()
{
	m_pauseFlg = false;

	if (m_wpTarget.expired() == false)
	{
		m_mWorld = m_mLocalPos * Math::Matrix::CreateTranslation(m_wpTarget.lock()->GetPos());
	}
}

void TPSCamera::SetPauseFlg(bool _pauseFlg)
{
	if (m_wpTarget.expired() == false)
	{
		// プレイヤーが生きている時だけポーズ処理をする
		if (m_wpTarget.lock()->GetAliveFlg() == true)
		{
			m_pauseFlg = _pauseFlg;
		}
	}
}

void TPSCamera::GoalProcess()
{
	m_goalProcess.stayCount++;
	if (m_goalProcess.stayCount > m_goalProcess.stayFrame)
	{
		m_goalProcess.frameCount++;

		if (m_goalProcess.frameCount < m_goalProcess.moveFrame)
		{
			m_goalProcess.nowPos += m_goalProcess.moveVec * m_goalProcess.speed;

			Math::Matrix targetMat = Math::Matrix::CreateTranslation(m_goalProcess.targetPos);

			Math::Matrix localMat = Math::Matrix::CreateTranslation(m_goalProcess.nowPos);

			m_goalProcess.nowDegAng += m_goalProcess.moveDegAng;

			Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_goalProcess.nowDegAng.x));

			m_mWorld = rotMat * localMat * targetMat;
		}
		else
		{
			Math::Matrix targetMat = Math::Matrix::CreateTranslation(m_goalProcess.targetPos);

			Math::Matrix localMat = Math::Matrix::CreateTranslation(m_goalProcess.endPos);

			Math::Vector3 targetPos = m_goalProcess.targetPos;

			Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_goalProcess.endDegAng.x));

			m_mWorld = rotMat * localMat * targetMat;

			m_goalProcess.moveEndFlg = true;
		}
	}
}