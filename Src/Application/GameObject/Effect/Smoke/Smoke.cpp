#include "Smoke.h"

void Smoke::Update()
{
	// ポーズ画面中は更新しない
	if (m_pauseFlg == true) return;

	switch (m_smokeType)
	{
	case SmokeType::WalkSmoke:
		WalkSmokeUpdate();
		break;

	case SmokeType::StampSmoke:
		StampSmokeUpdate();
		break;

	case SmokeType::DeathSmoke:
		DeathSmokeUpdate();
		break;

	case SmokeType::AppearanceSmoke:
		AppearanceSmokeUpdate();
		break;
	}
}

void Smoke::DrawLit()
{
	if (m_spModel)
	{
		for (Math::Vector3& pos : m_posList)
		{
			Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);

			Math::Matrix transMat = Math::Matrix::CreateTranslation(pos);

			m_mWorld = scaleMat * transMat;

			KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
		}
	}
}

void Smoke::Init()
{
	srand(timeGetTime());

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Effect/Smoke/smoke.gltf");
	}

	m_scale = 1.0f;
}

void Smoke::Reset()
{
	m_isExpired = true;
}

void Smoke::SetSmokeType(SmokeType _smokeType)
{
	m_smokeType = _smokeType;

	switch (m_smokeType)
	{
	case SmokeType::WalkSmoke:
		m_smokeNum = m_walkSmoke.smokeNum;
		break;

	case SmokeType::StampSmoke:
		m_smokeNum = m_stampSmoke.smokeNum;
		break;

	case SmokeType::DeathSmoke:
		m_smokeNum = m_deathSmoke.smokeNum;
		break;
		
	case SmokeType::AppearanceSmoke:
		m_smokeNum = m_appearanceSmoke.smokeNum;

		for (int i = 0; i < m_smokeNum; i++)
		{
			m_appearanceSmoke.moveVec.push_back({ float(rand() % 200 - 100), 0, float(rand() % 200 - 100) });
			m_appearanceSmoke.moveVec[i].Normalize();
			m_appearanceSmoke.movePow.push_back(float(rand() % 100 / 1000.0f));
		}

		m_scale = 4.0f;

		break;
	}

	for (int i = 0; i < m_smokeNum; i++)
	{
		m_posList.push_back(m_basePos);
	}
}

void Smoke::WalkSmokeUpdate()
{
	// 少しずつ小さくしていく
	// scaleが0になったら消す
	m_scale -= m_walkSmoke.subScale;
	if (m_scale <= 0)
	{
		m_isExpired = true;
		m_scale = 0;
	}
}

void Smoke::StampSmokeUpdate()
{
	// 小さくするまでのカウント	
	m_stampSmoke.subCount++;
	// 時間になったら小さくしていく
	if (m_stampSmoke.subCount > m_stampSmoke.subTime)
	{
		m_scale -= m_stampSmoke.subScale;
		if (m_scale <= 0)
		{
			m_isExpired = true;
			m_scale = 0;
		}
	}

	int cnt = 0;
	// 少し移動させる
	for (auto& pos : m_posList)
	{
		Math::Vector3 moveVec;
		float rotRadAng = DirectX::XMConvertToRadians(float(cnt * (360 / m_smokeNum)));
		moveVec = moveVec.TransformNormal(m_stampSmoke.moveVec, Math::Matrix::CreateRotationY(rotRadAng));
		
		pos = Math::Vector3::Lerp(pos, m_basePos + moveVec * m_stampSmoke.movePow, 0.2f);
		
		cnt++;
	}
}

void Smoke::DeathSmokeUpdate()
{
	// 小さくするまでのカウント	
	m_deathSmoke.subCount++;
	// 時間になったら小さくしていく
	if (m_deathSmoke.subCount > m_deathSmoke.subTime)
	{
		m_scale -= m_deathSmoke.subScale;
		if (m_scale <= 0)
		{
			m_isExpired = true;
			m_scale = 0;
		}
	}

	int cnt = 0;
	// 少し移動させる
	for (auto& pos : m_posList)
	{
		Math::Vector3 moveVec;
		float rotRadAng = DirectX::XMConvertToRadians(float(cnt * (360 / m_smokeNum)));
		moveVec = moveVec.TransformNormal(m_deathSmoke.moveVec, Math::Matrix::CreateRotationY(rotRadAng));

		pos = Math::Vector3::Lerp(pos, m_basePos + moveVec * m_deathSmoke.movePow, 0.2f);

		cnt++;
	}
}

void Smoke::AppearanceSmokeUpdate()
{
	// 小さくするまでのカウント	
	m_appearanceSmoke.subCount++;
	// 時間になったら小さくしていく
	if (m_appearanceSmoke.subCount > m_appearanceSmoke.subTime)
	{
		m_scale -= m_appearanceSmoke.subScale;
		if (m_scale <= 0)
		{
			m_isExpired = true;
			m_scale = 0;
		}
	}

	int cnt = 0;
	// 少し移動させる
	for (auto& pos : m_posList)
	{
		pos += m_appearanceSmoke.moveVec[cnt] * m_appearanceSmoke.movePow[cnt];

		pos.y += m_appearanceSmoke.upMovePow;

		cnt++;
	}
	m_appearanceSmoke.upMovePow -= m_appearanceSmoke.downMovePow;
}
