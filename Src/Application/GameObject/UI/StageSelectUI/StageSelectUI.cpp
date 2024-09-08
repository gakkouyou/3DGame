#include "StageSelectUI.h"
#include "../../StageSelectTexture/StageSelectTexture.h"

void StageSelectUI::Update()
{
	// 三角の画像のアルファ値を変えていく
	m_triangle.sinCurve.degAng++;
	if (m_triangle.sinCurve.degAng >= m_triangle.sinCurve.maxDegAng)
	{
		m_triangle.sinCurve.degAng -= m_triangle.sinCurve.maxDegAng - m_triangle.sinCurve.minDegAng;
	}
	m_triangle.alpha = sin(DirectX::XMConvertToRadians(m_triangle.sinCurve.degAng));
}

void StageSelectUI::DrawSprite()
{
	std::shared_ptr<StageSelectTexture> spStage = m_wpStage.lock();
	if (!spStage) return;

	int nowStage = spStage->GetNowStage();
	int maxStage = spStage->GetMaxStage();
	int stageInfo = spStage->GetStageInfo();

	// 三角
	if (m_triangle.spTex)
	{
		// 右三角
		if (nowStage < maxStage)
		{
			Math::Matrix rotMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(-90.0f));
			Math::Matrix transMat = Math::Matrix::CreateTranslation(m_triangle.pos.x, m_triangle.pos.y, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(rotMat * transMat);
			Math::Color color = { 1, 1, 1, m_triangle.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_triangle.spTex, 0, 0, nullptr, &color);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		}
		// 左三角
		if (nowStage != 0)
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
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stage.spTex, (int)m_stage.pos.x, (int)m_stage.pos.y);
	}

	// 数字
	if (m_number.spTex)
	{
		Math::Vector2 size = { (float)m_number.spTex->GetWidth() / 10, (float)m_number.spTex->GetHeight() };
		Math::Rectangle rect = { (long)size.x * (nowStage + 1), (long)0, (long)size.x, (long)size.y };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_number.spTex, (int)m_number.pos.x, (int)m_number.pos.y, (int)size.x, (int)size.y, &rect);
	}

	// 挑戦不可能以外なら描画する
	if (stageInfo != StageSelectTexture::StageInfo::ImPossible)
	{
		// 白い画像
		if (m_white.spTex)
		{
			/*Math::Color color = { 1, 1, 1, m_white.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_white.spTex, m_white.pos.x, m_white.pos.y, nullptr, &color);*/
		}

		// "PUSH SPACE"
		if (m_space.spTex)
		{
			Math::Color color = { 1, 1, 1, m_triangle.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_space.spTex, (int)m_space.pos.x, (int)m_space.pos.y, nullptr, &color);
		}
	}

	// ステージをクリアしているなら、Clear!を描画する
	if (stageInfo == StageSelectTexture::StageInfo::Clear)
	{
		if (m_clear.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear.spTex, (int)m_clear.pos.x, (int)m_clear.pos.y);
		}
	}

	// 挑戦不可能なら黒い画像を半透明に描画する
	if (stageInfo == StageSelectTexture::StageInfo::ImPossible)
	{
		if (m_black.spTex)
		{
			Math::Color color = { 1, 1, 1, m_black.alpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_black.spTex, 0, 0, nullptr, &color);
		}
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
		m_number.spTex->Load("Asset/Textures/StageSelect/UI/number.png");
	}
	m_number.pos = { -370, 305 };

	// "PUSH SPACE"
	if (!m_space.spTex)
	{
		m_space.spTex = std::make_shared<KdTexture>();
		m_space.spTex->Load("Asset/Textures/StageSelect/UI/space.png");
	}
	m_space.pos = { 0, -250 };

	// プッシュスペースを見やすくするための白い画像
	if (!m_white.spTex)
	{
		m_white.spTex = std::make_shared<KdTexture>();
		m_white.spTex->Load("Asset/Textures/StageSelect/UI/white.png");
	}
	m_white.pos = m_space.pos;
	m_white.alpha = 0.7f;

	// "Clear!"
	if (!m_clear.spTex)
	{
		m_clear.spTex = std::make_shared<KdTexture>();
		m_clear.spTex->Load("Asset/Textures/StageSelect/UI/clear.png");
	}
	m_clear.pos = { 500, 300 };

	// 黒い画像
	if (!m_black.spTex)
	{
		m_black.spTex = std::make_shared<KdTexture>();
		m_black.spTex->Load("Asset/Textures/Scene/black.png");
	}
	m_black.alpha = 0.5f;

	//// ステージ数の枠
	//if (!m_frame.spTex)
	//{
	//	m_frame.spTex = std::make_shared<KdTexture>();
	//	m_frame.spTex->Load("Asset/Textures/StageSelect/UI/frame2.png");
	//}
	//m_frame.pos = { 0, -200 };
}
