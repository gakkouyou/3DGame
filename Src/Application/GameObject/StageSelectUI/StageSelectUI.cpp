#include "StageSelectUI.h"

void StageSelectUI::Update()
{
	m_triangle.sinCurve.degAng++;
	if (m_triangle.sinCurve.degAng >= m_triangle.sinCurve.maxDegAng)
	{
		m_triangle.sinCurve.degAng -= m_triangle.sinCurve.maxDegAng - m_triangle.sinCurve.minDegAng;
	}
	m_triangle.alpha = sin(DirectX::XMConvertToRadians(m_triangle.sinCurve.degAng));
}

void StageSelectUI::DrawSprite()
{
	// 三角
	if (m_triangle.spTex)
	{
		// 右三角
		if (m_nowStage != m_maxStage)
		{
			Math::Matrix rotMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(-90.0f));
			Math::Matrix transMat = Math::Matrix::CreateTranslation(m_triangle.pos.x, m_triangle.pos.y, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(rotMat * transMat);
			Math::Color color = { 1, 1, 1, m_triangle.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_triangle.spTex, 0, 0, nullptr, &color);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		}
		// 左三角
		if (m_nowStage != 0)
		{
			Math::Matrix rotMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(90.0f));
			Math::Matrix transMat = Math::Matrix::CreateTranslation(-m_triangle.pos.x, m_triangle.pos.y, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(rotMat * transMat);
			Math::Color color = { 1, 1, 1, m_triangle.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_triangle.spTex, 0, 0, nullptr, &color);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		}
	}

	// "Stage"
	if (m_stage.spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stage.spTex, m_stage.pos.x, m_stage.pos.y);
	}

	// 数字
	if (m_number.spTex)
	{
		Math::Vector2 size = { (float)m_number.spTex->GetWidth() / 10, (float)m_number.spTex->GetHeight() };
		Math::Rectangle rect = { (long)size.x * (m_nowStage + 1), (long)0, (long)size.x, (long)size.y };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_number.spTex, m_number.pos.x, m_number.pos.y, size.x, size.y, &rect);
	}

	// 白い画像
	if (m_white.spTex)
	{
		Math::Color color = { 1, 1, 1, m_white.alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_white.spTex, m_white.pos.x, m_white.pos.y, nullptr, &color);
	}

	// "PUSH SPACE"
	if (m_space.spTex)
	{
		Math::Color color = { 1, 1, 1, m_triangle.alpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_space.spTex, m_space.pos.x, m_space.pos.y, nullptr, &color);
	}

	//if (m_frame.spTex)
	//{
	//	KdShaderManager::Instance().m_spriteShader.DrawTex(m_frame.spTex, m_frame.pos.x, m_frame.pos.y);
	//}
}

void StageSelectUI::Init()
{
	// 三角
	if (!m_triangle.spTex)
	{
		m_triangle.spTex = std::make_shared<KdTexture>();
		m_triangle.spTex->Load("Asset/Textures/StageSelect/UI/triangle.png");
	}
	m_triangle.pos					= { 550, 0 };
	m_triangle.sinCurve.addAng		= 1;
	m_triangle.sinCurve.maxDegAng	= 160;
	m_triangle.sinCurve.minDegAng	= 20;
	m_triangle.sinCurve.degAng		= m_triangle.sinCurve.minDegAng;

	// ステージ
	if (!m_stage.spTex)
	{
		m_stage.spTex = std::make_shared<KdTexture>();
		m_stage.spTex->Load("Asset/Textures/StageSelect/UI/stage.png");
	}
	m_stage.pos = { -500, 300 };

	// 数字
	if (!m_number.spTex)
	{
		m_number.spTex = std::make_shared<KdTexture>();
		m_number.spTex->Load("Asset/Textures/StageSelect/UI/number - コピー.png");
	}
	m_number.pos = { -370, 305 };

	// "PUSH SPACE"
	if (!m_space.spTex)
	{
		m_space.spTex = std::make_shared<KdTexture>();
		m_space.spTex->Load("Asset/Textures/StageSelect/UI/space.png");
	}
	m_space.pos = { 0, -250 };

	if (!m_white.spTex)
	{
		m_white.spTex = std::make_shared<KdTexture>();
		m_white.spTex->Load("Asset/Textures/StageSelect/UI/white.png");
	}
	m_white.pos = m_space.pos;
	m_white.alpha = 0.7f;



	//// ステージ数の枠
	//if (!m_frame.spTex)
	//{
	//	m_frame.spTex = std::make_shared<KdTexture>();
	//	m_frame.spTex->Load("Asset/Textures/StageSelect/UI/frame2.png");
	//}
	//m_frame.pos = { 0, -200 };
}
