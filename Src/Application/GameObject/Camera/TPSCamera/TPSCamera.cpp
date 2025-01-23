#include "TPSCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"
#include "../../../main.h"

void TPSCamera::PostUpdate()
{
	static Math::Matrix mat;

	static bool flg = false;
	if (SceneManager::Instance().GetDebug() == true)
	{
		if (flg == false)
		{
			mat = m_mWorld;
		}
		flg = true;
	}
	else
	{
		flg = false;
		mat = m_mWorld;
	}

	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}

	CameraBase::Update();

	// しすいだいかりんぐ
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, m_spCamera->GetProjMatrix());
	frustum.Transform(frustum, mat);

	int count = 0;
	int drawCount = 0;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj->GetBaseObjectType() == BaseObjectType::Ground)
		{
			if (obj->CheckInScreen(frustum))
			{
				obj->SetDrawFlg(true);
				drawCount++;
			}
			else
			{
				obj->SetDrawFlg(false);
			}
			count++;
		}
	}
}

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// ターゲットからの行列
	Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 4.5f, -6.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(20.0f));
	m_mLocalPos = rotMat * transMat;

	// 最初は完全に追尾した状態から始まる
	if (m_wpPlayer.expired() == false)
	{
		m_mWorld = m_mLocalPos * Math::Matrix::CreateTranslation(m_wpPlayer.lock()->GetPos());
	}

	// 初クリア時の演出の際のターゲットからの行列
	m_localGoalMat = rotMat;
	m_localGoalMat.Translation({ 0, 3.5f, -8.0f });

	// 初クリア時の最終ゴールを見る時の行列
	m_finalGoalMat.Translation({ 0, 5.5f, -10.0f });

	// ゴール演出のもろもろ
	m_goalProcess.startPos		= { 0, 1, -2.25 };	// 初期座標
	m_goalProcess.endPos		= { 0, 3.5, -8 };	// 最終的な座標
	m_goalProcess.speed			= 0.007f;			// 速度
	m_goalProcess.startDegAng	= { 0, 0, 0 };		// 初期角度
	m_goalProcess.endDegAng		= { 20, 0, 0 };		// 最終的な角度
	m_goalProcess.stayFrame		= 60;				// 演出が終わってからの待機時間

	m_nowAction = std::make_shared<Tracking>();
}

void TPSCamera::Reset()
{
	m_pauseFlg = false;

	if (m_wpPlayer.expired() == false)
	{
		m_mWorld = m_mLocalPos * Math::Matrix::CreateTranslation(m_wpPlayer.lock()->GetPos());
	}
}

void TPSCamera::SetPauseFlg(bool _pauseFlg)
{
	if (m_wpPlayer.expired() == false)
	{
		// プレイヤーが生きている時だけポーズ処理をする
		if (m_wpPlayer.lock()->GetAliveFlg() == true)
		{
			m_pauseFlg = _pauseFlg;
		}
	}
}

void TPSCamera::GoalProcess()
{
	if (m_wpTarget.expired() == true) return;
	m_goalProcess.targetPos = m_wpTarget.lock()->GetPos();

	m_goalProcess.stayCount++;
	if (m_goalProcess.stayCount > m_goalProcess.stayFrame)
	{
		// 回転の補完
		// スタートの行列
		Math::Matrix startMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_goalProcess.startDegAng.y), DirectX::XMConvertToRadians(m_goalProcess.startDegAng.x), DirectX::XMConvertToRadians(m_goalProcess.startDegAng.z));
		// ゴールの行列
		Math::Matrix goalMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_goalProcess.endDegAng.y), DirectX::XMConvertToRadians(m_goalProcess.endDegAng.x), DirectX::XMConvertToRadians(m_goalProcess.endDegAng.z));

		// クォータニオン用意
		Math::Quaternion startQue;
		Math::Quaternion endQue;
		Math::Quaternion nowQue;

		// 最初の行列からクォータニオン作成
		startQue = Math::Quaternion::CreateFromRotationMatrix(startMat);
		endQue = Math::Quaternion::CreateFromRotationMatrix(goalMat);

		// 進行具合を加味した回転を求める
		// 球面線形補完
		nowQue = Math::Quaternion::Slerp(startQue, endQue, m_goalProcess.progress);

		// ワールド行列を更新（回転）
		m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

		// 座標の補完
		Math::Vector3 startVec;
		Math::Vector3 endVec;

		startVec = m_goalProcess.startPos + m_goalProcess.targetPos;
		endVec = m_goalProcess.endPos + m_goalProcess.targetPos;

		// 進行具合を加味した座標を求める
		Math::Vector3 nowVec;

		// 線形補完
		nowVec = Math::Vector3::Lerp(startVec, endVec, m_goalProcess.progress);

		// ワールド行列を更新(座標)
		m_mWorld.Translation(nowVec);

		//=========================================
		// 進行具合の更新
		//=========================================
		m_goalProcess.progress += m_goalProcess.speed;

		if (m_goalProcess.progress >= 1.0f)
		{
			m_goalProcess.progress = 1.0f;
			m_goalProcess.moveEndFlg = true;
		}
	}
}

void TPSCamera::FirstClearProcess()
{
	//// 回転の補完
	//Math::Matrix startMat = m_startMat;

	//// ゴールの回転行列
	//Math::Matrix goalRotMat = m_goalMat;
	//goalRotMat.Translation({ 0, 0, 0 });
	//// ゴールの座標行列
	//Math::Matrix goalTransMat = Math::Matrix::CreateTranslation(m_goalMat.Translation());
	//// ターゲットの回転行列
	//Math::Matrix targetRotMat = m_targetMat;
	//targetRotMat.Translation({ 0, 0, 0 });
	//// ターゲットの座標行列
	//Math::Matrix targetTransMat = Math::Matrix::CreateTranslation(m_targetMat.Translation());
	//
	//// ゴールの座標行列をターゲットの回転行列で補正
	//goalTransMat = Math::Matrix::CreateTranslation(Math::Vector3::TransformNormal(goalTransMat.Translation(), targetRotMat));

	//// ゴールの行列
	//Math::Matrix goalMat = goalRotMat * targetRotMat * goalTransMat * targetTransMat;

	//// クォータニオン用意
	//Math::Quaternion startQue;
	//Math::Quaternion endQue;
	//Math::Quaternion nowQue;

	//// 最初の行列からクォータニオン作成
	//startQue = Math::Quaternion::CreateFromRotationMatrix(startMat);
	//endQue = Math::Quaternion::CreateFromRotationMatrix(goalRotMat * targetRotMat);

	//// 進行具合を加味した回転を求める
	//// 球面線形補完
	//nowQue = Math::Quaternion::Slerp(startQue, endQue, m_progress);

	//// ワールド行列を更新（回転）
	//m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

	//// 座標の補完
	//Math::Vector3 startVec;
	//Math::Vector3 endVec;

	//startVec = startMat.Translation();
	//endVec = goalMat.Translation();

	//// 進行具合を加味した座標を求める
	//Math::Vector3 nowVec;

	//// 線形補完
	//nowVec = Math::Vector3::Lerp(startVec, endVec, m_progress);

	//// ワールド行列を更新(座標)
	//m_mWorld.Translation(nowVec);

	////=========================================
	//// 進行具合の更新
	////=========================================
	//if (m_firstClear & FirstClear::NowStartProcess)
	//{
	//	m_progress += m_addProgress;

	//	if (m_progress > 1.0f)
	//	{
	//		m_progress = 1.0f;
	//		m_firstClear |= FirstClear::StartProgressMax;
	//	}
	//}
	//else if (m_firstClear & FirstClear::NowEndProcess)
	//{
	//	m_progress -= m_addProgress;

	//	if (m_progress < 0.0f)
	//	{
	//		m_progress = 0.0f;
	//		m_firstClear |= FirstClear::EndProgressMax;
	//	}
	//}
}

void TPSCamera::ChangeActionState(std::shared_ptr<StateBase> _nextState)
{
	if (m_nowAction) m_nowAction->Exit(*this);
	m_nowAction = _nextState;
	m_nowAction->Enter(*this);
}

void TPSCamera::Tracking::Enter(TPSCamera& _owner)
{
}

void TPSCamera::Tracking::Update(TPSCamera& _owner)
{
	// デバッグモードになったらデバッグモードに切替
	if (SceneManager::Instance().GetDebug())
	{
		_owner.ChangeActionState(std::make_shared<Debug>());
		return;
	}

	// ポーズ画面になったらポーズモードに切り替え
	if (_owner.m_pauseFlg)
	{
		_owner.ChangeActionState(std::make_shared<Pause>());
	}

	// ターゲットの行列(有効な場合利用する)
	const std::shared_ptr<const Player>	spTarget = _owner.m_wpPlayer.lock();
	if (!spTarget) return;
	// ターゲットの座標
	Math::Vector3	targetPos = spTarget->GetPos();
	// ターゲットの座標行列
	Math::Matrix	targetMat = Math::Matrix::CreateTranslation(targetPos);

	// 追尾すべきかすべきでないか
	bool trackingFlg = spTarget->IsCameraTracking();

	// 追尾すべきでない場合(ジャンプしただけ、箱に乗っただけ、柵に乗っただけ等)
	if (trackingFlg == false)
	{
		// 一定以上上に上がった時追尾する
		if (targetPos.y - _owner.m_oldPlayerPos.y >= _owner.m_trackingUp)
		{
			targetPos.y = _owner.m_oldPlayerPos.y + (targetPos.y - _owner.m_oldPlayerPos.y) - _owner.m_trackingUp;
		}
		// 一定以上下に下がった時追尾する
		else if (targetPos.y - _owner.m_oldPlayerPos.y <= -_owner.m_trackingDown)
		{
			targetPos.y = _owner.m_oldPlayerPos.y + (targetPos.y - _owner.m_oldPlayerPos.y) + _owner.m_trackingDown;
		}
		// 追尾しない
		else
		{
			targetPos.y = _owner.m_oldPlayerPos.y;
		}
	}

	// 滑らかに動くようにする
	targetMat.Translation(Math::Vector3::Lerp(_owner.m_mWorld.Translation() - _owner.m_mLocalPos.Translation(), targetPos, _owner.m_trackingLerp));

	// 行列確定
	_owner.m_mWorld = _owner.m_mLocalPos * targetMat;

	// 追尾している時のみプレイヤーの座標を保持しておく
	if (trackingFlg == true)
	{
		_owner.m_oldPlayerPos = targetPos;
	}
}

void TPSCamera::Goal::Update(TPSCamera& _owner)
{
	// もし動き終わっていたら終了
	if (_owner.m_goalProcess.moveEndFlg == true) return;

	// ターゲットがいなかったら終了
	if (_owner.m_wpTarget.expired() == true) return;
	// ターゲットの座標
	_owner.m_goalProcess.targetPos = _owner.m_wpTarget.lock()->GetPos();

	// ゴールしてからの待機時間を進める
	_owner.m_goalProcess.stayCount++;
	// 待機中なら終了
	if (_owner.m_goalProcess.stayCount < _owner.m_goalProcess.stayFrame) return;

	//===============================================
	// ゴール演出
	//===============================================
	// 回転の補完
	// スタートの行列
	Math::Matrix startMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(_owner.m_goalProcess.startDegAng.y), DirectX::XMConvertToRadians(_owner.m_goalProcess.startDegAng.x), DirectX::XMConvertToRadians(_owner.m_goalProcess.startDegAng.z));
	// ゴールの行列
	Math::Matrix goalMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(_owner.m_goalProcess.endDegAng.y), DirectX::XMConvertToRadians(_owner.m_goalProcess.endDegAng.x), DirectX::XMConvertToRadians(_owner.m_goalProcess.endDegAng.z));

	// クォータニオン用意
	Math::Quaternion startQue;
	Math::Quaternion endQue;
	Math::Quaternion nowQue;

	// 最初の行列からクォータニオン作成
	startQue = Math::Quaternion::CreateFromRotationMatrix(startMat);
	endQue = Math::Quaternion::CreateFromRotationMatrix(goalMat);

	// 進行具合を加味した回転を求める
	// 球面線形補完
	nowQue = Math::Quaternion::Slerp(startQue, endQue, _owner.m_goalProcess.progress);

	// ワールド行列を更新（回転）
	_owner.m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

	// 座標の補完
	Math::Vector3 startVec;
	Math::Vector3 endVec;

	startVec = _owner.m_goalProcess.startPos + _owner.m_goalProcess.targetPos;
	endVec = _owner.m_goalProcess.endPos + _owner.m_goalProcess.targetPos;

	// 進行具合を加味した座標を求める
	Math::Vector3 nowVec;

	// 線形補完
	nowVec = Math::Vector3::Lerp(startVec, endVec, _owner.m_goalProcess.progress);

	// ワールド行列を更新(座標)
	_owner.m_mWorld.Translation(nowVec);

	//=========================================
	// 進行具合の更新
	//=========================================
	_owner.m_goalProcess.progress += _owner.m_goalProcess.speed;

	// 演出終了
	if (_owner.m_goalProcess.progress >= 1.0f)
	{
		_owner.m_goalProcess.progress = 1.0f;
		_owner.m_goalProcess.moveEndFlg = true;
		return;
	}
}

void TPSCamera::Goal::Exit(TPSCamera& _owner)
{
}

void TPSCamera::FinalGoal::Enter(TPSCamera& _owner)
{
	// ターゲットの行列
	if (_owner.m_wpTarget.expired() == false)
	{
		_owner.m_targetMat = _owner.m_wpTarget.lock()->GetMatrix();
	}

	// 最初の行列を保持
	_owner.m_startMat = _owner.m_mWorld;
	// 最終的な行列を作成
	// ゴールの回転行列
	Math::Matrix goalRotMat = _owner.m_localGoalMat;
	goalRotMat.Translation({ 0, 0, 0 });
	// ゴールの座標行列
	Math::Matrix goalTransMat = Math::Matrix::CreateTranslation(_owner.m_finalGoalMat.Translation());
	// ターゲットの座標行列
	Math::Matrix targetTransMat = Math::Matrix::CreateTranslation(_owner.m_targetMat.Translation());

	// ゴールの行列
	_owner.m_goalMat = goalRotMat * goalTransMat * targetTransMat;

	// 最初のクォータニオン作成
	_owner.m_startQua = Math::Quaternion::CreateFromRotationMatrix(_owner.m_startMat);
	// 最終的なクォータニオン作成
	_owner.m_goalQua = Math::Quaternion::CreateFromRotationMatrix(goalRotMat);
}

void TPSCamera::FinalGoal::Update(TPSCamera& _owner)
{
	// 進行度が上限に行った時の処理
	if (_owner.m_progress > 1.0f)
	{
		// 進行度を上限にする
		_owner.m_progress = 1.0f;
		// 最終ゴール到着
		_owner.m_firstClear = FirstClear::FinalGoalProcessEnd;

		return;
	}

	// 進行具合を加味した回転を求める
	// 球面線形補完
	Math::Quaternion nowQue = Math::Quaternion::Slerp(_owner.m_startQua, _owner.m_goalQua, _owner.m_progress);

	// ワールド行列を更新（回転）
	_owner.m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

	// 座標の補完
	Math::Vector3 startVec;
	Math::Vector3 endVec;

	startVec = _owner.m_startMat.Translation();
	endVec = _owner.m_goalMat.Translation();

	// 進行具合を加味した座標を求める
	Math::Vector3 nowVec;

	// 線形補完
	nowVec = Math::Vector3::Lerp(startVec, endVec, _owner.m_progress);

	// ワールド行列を更新(座標)
	_owner.m_mWorld.Translation(nowVec);

	// 進行度を進める
	_owner.m_progress += _owner.m_addProgress;
}

void TPSCamera::NewRoad::Enter(TPSCamera& _owner)
{
	// ターゲットの行列
	if (_owner.m_wpTarget.expired() == false)
	{
		_owner.m_targetMat = _owner.m_wpTarget.lock()->GetMatrix();
	}

	// ゴールの回転行列
	Math::Matrix goalRotMat = _owner.m_localGoalMat;
	goalRotMat.Translation({ 0, 0, 0 });
	// ゴールの座標行列
	Math::Matrix goalTransMat = Math::Matrix::CreateTranslation(_owner.m_localGoalMat.Translation());
	// ターゲットの回転行列
	Math::Matrix targetRotMat = _owner.m_targetMat;
	targetRotMat.Translation({ 0, 0, 0 });
	// ターゲットの座標行列
	Math::Matrix targetTransMat = Math::Matrix::CreateTranslation(_owner.m_targetMat.Translation());

	// ゴールの座標行列をターゲットの回転行列で補正
	goalTransMat = Math::Matrix::CreateTranslation(Math::Vector3::TransformNormal(goalTransMat.Translation(), targetRotMat));

	// 最初の行列
	_owner.m_startMat = _owner.m_goalMat;
	// ゴールの行列
	_owner.m_goalMat = goalRotMat * targetRotMat * goalTransMat * targetTransMat;

	// 最初の行列からクォータニオン作成
	_owner.m_startQua = Math::Quaternion::CreateFromRotationMatrix(_owner.m_startMat);
	_owner.m_goalQua = Math::Quaternion::CreateFromRotationMatrix(goalRotMat * targetRotMat);

	_owner.m_progress = 0.0f;
}

void TPSCamera::NewRoad::Update(TPSCamera& _owner)
{
	// 進行度が上限に行った時の処理
	if (_owner.m_progress > 1.0f)
	{
		// 進行度を上限にする
		_owner.m_progress = 1.0f;
		// 新たな道到着
		_owner.m_firstClear = FirstClear::SlopeProcessEnd;

		return;
	}

	// 進行具合を加味した回転を求める
	// 球面線形補完
	Math::Quaternion nowQue = Math::Quaternion::Slerp(_owner.m_startQua, _owner.m_goalQua, _owner.m_progress);

	// ワールド行列を更新（回転）
	_owner.m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

	// 座標の補完
	Math::Vector3 startVec;
	Math::Vector3 endVec;

	startVec = _owner.m_startMat.Translation();
	endVec = _owner.m_goalMat.Translation();

	// 進行具合を加味した座標を求める
	Math::Vector3 nowVec;

	// 線形補完
	nowVec = Math::Vector3::Lerp(startVec, endVec, _owner.m_progress);

	// ワールド行列を更新(座標)
	_owner.m_mWorld.Translation(nowVec);

	// 進行度を進める
	_owner.m_progress += _owner.m_addProgress;
}

void TPSCamera::Back::Enter(TPSCamera& _owner)
{
	// 進行度をリセット
	_owner.m_progress = 0.0f;

	Math::Matrix playerMat;
	// ゴールはプレイヤーの所
	if (_owner.m_wpPlayer.expired() == false)
	{
		playerMat = Math::Matrix::CreateTranslation(_owner.m_wpPlayer.lock()->GetPos());
	}

	// クォータニオン作成
	_owner.m_startQua = Math::Quaternion::CreateFromRotationMatrix(_owner.m_goalMat);
	Math::Matrix goalRotMat = _owner.m_mLocalPos * playerMat;
	goalRotMat.Translation({ 0, 0, 0 });
	_owner.m_goalQua = Math::Quaternion::CreateFromRotationMatrix(goalRotMat);

	// スタートは坂の所
	_owner.m_startMat = _owner.m_goalMat;
	// ゴールはプレイヤーの所
	_owner.m_goalMat = _owner.m_mLocalPos * playerMat;
}

void TPSCamera::Back::Update(TPSCamera& _owner)
{
	// 元に戻ったら追尾状態になる
	if (_owner.m_progress >= 1.0f)
	{
		_owner.m_firstClear = FirstClear::FirstClearEnd;
		_owner.ChangeActionState(std::make_shared<Tracking>());
		return;
	}

	// 進行具合を加味した回転を求める
	// 球面線形補完
	Math::Quaternion nowQue = Math::Quaternion::Slerp(_owner.m_startQua, _owner.m_goalQua, _owner.m_progress);

	// ワールド行列を更新（回転）
	_owner.m_mWorld = Math::Matrix::CreateFromQuaternion(nowQue);

	// 座標の補完
	Math::Vector3 startVec;
	Math::Vector3 endVec;

	startVec = _owner.m_startMat.Translation();
	endVec = _owner.m_goalMat.Translation();

	// 進行具合を加味した座標を求める
	Math::Vector3 nowVec;

	// 線形補完
	nowVec = Math::Vector3::Lerp(startVec, endVec, _owner.m_progress);

	// ワールド行列を更新(座標)
	_owner.m_mWorld.Translation(nowVec);

	// 進行度を進める
	_owner.m_progress += _owner.m_addProgress;
}

void TPSCamera::Pause::Enter(TPSCamera& _owner)
{
	_owner.m_spCamera->SetFocus(0, 0, _owner.m_pauseFocus);
}

void TPSCamera::Pause::Update(TPSCamera& _owner)
{
	// ポーズ画面解除
	if (!_owner.m_pauseFlg)
	{
		_owner.ChangeActionState(std::make_shared<Tracking>());
	}
}

void TPSCamera::Pause::Exit(TPSCamera& _owner)
{
	_owner.m_spCamera->SetFocus(0, 0, _owner.m_defaultFocus);
}

void TPSCamera::Debug::Enter(TPSCamera& _owner)
{
	_owner.m_debugPos	= Math::Vector3::Zero;
	_owner.m_DegAng		= Math::Vector3::Zero;
}

void TPSCamera::Debug::Update(TPSCamera& _owner)
{
	// デバッグモードが解除されたら追尾に切替
	if (SceneManager::Instance().GetDebug() == false)
	{
		_owner.ChangeActionState(std::make_shared<Tracking>());
		return;
	}

	Math::Matrix debugMat	= Math::Matrix::Identity;
	_owner.m_moveVec		= Math::Vector3::Zero;

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (!_owner.m_shiftFlg)
		{
			_owner.m_shiftFlg = true;
			SetCursorPos(_owner.m_FixMousePos.x, _owner.m_FixMousePos.y);
		}
		else
		{
			_owner.UpdateRotateByMouse();
		}
	}
	else
	{
		_owner.m_shiftFlg = false;
	}

	// WASDで移動
	if (GetAsyncKeyState('W') & 0x8000)
	{
		_owner.m_moveVec.z += 1.0f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		_owner.m_moveVec.x -= 1.0f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		_owner.m_moveVec.z -= 1.0f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		_owner.m_moveVec.x += 1.0f;
	}

	// カメラの向きで移動方向を補正
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(_owner.m_DegAng.y), 0, DirectX::XMConvertToRadians(_owner.m_DegAng.z));
	_owner.m_moveVec = _owner.m_moveVec.TransformNormal(_owner.m_moveVec, rotMat);
	_owner.m_moveVec.Normalize();
	_owner.m_moveVec.y = 0;

	// スペースで上に行く
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		_owner.m_moveVec.y += 1.0f;
	}
	// controlで下に行く
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		_owner.m_moveVec.y -= 1.0f;
	}


	_owner.m_debugPos += _owner.m_moveVec * _owner.m_moveSpeed;

	debugMat = Math::Matrix::CreateTranslation(_owner.m_debugPos);

	// カメラの回転
	_owner.m_mRotation = _owner.GetRotationMatrix();

	// ローカル行列変更
	Math::Matrix transMat = Math::Matrix::CreateTranslation(0, 3.0f, -5.0f);

	// ターゲットの行列(有効な場合利用する)
	const std::shared_ptr<const Player>	spTarget = _owner.m_wpPlayer.lock();
	Math::Matrix targetMat = Math::Matrix::Identity;
	if (spTarget)
	{
		// ターゲットの座標
		Math::Vector3	targetPos = spTarget->GetPos();
		// ターゲットの座標行列
		targetMat = Math::Matrix::CreateTranslation(targetPos);
	}

	_owner.m_mWorld = _owner.m_mRotation * transMat * targetMat * debugMat;
}

void TPSCamera::Debug::Exit(TPSCamera& _owner)
{
	_owner.m_debugPos = Math::Vector3::Zero;
	_owner.m_DegAng = Math::Vector3::Zero;
}
