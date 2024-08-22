#include "Result.h"
#include "Application/Utility.h"

void Result::Update()
{
	// クリアの時の処理
	if (m_clearFlg)
	{
		m_clear.alpha += m_clear.alphaAdd;
		if (m_clear.alpha > 1.0f)
		{
			m_clear.alpha = 1.0f;
			m_clearStayCount++;
		}
		if (m_clearStayCount > m_clearStayTime)
		{
			m_clearFinishFlg = true;
		}
	}

	// ゲームオーバー時の処理
	if (m_gameOverFlg)
	{
		m_miss.move.y -= 1.0f;
		if (m_miss.move.y < -20.0f)
		{
			m_miss.move.y = -20.0f;
		}

		m_miss.pos.y += m_miss.move.y;
		if (m_miss.pos.y < -200.0f)
		{
			switch (m_boundCnt)
			{
			case 0:
				m_miss.move.y = 10.0f;
				break;

			case 1:
				m_miss.move.y = 1.0f;
				break;

			case 2:
				m_miss.move.y = 0;
				break;
			}
			m_miss.pos.y = -200.0f;
			m_boundCnt++;
		}
	}
}

void Result::DrawSprite()
{
	if (m_clearFlg)
	{
		if (m_clear.spTex)
		{
			Math::Color color = { 1, 1, 1, m_clear.alpha };
			Math::Vector2 size = { (float)m_clear.spTex->GetWidth(), (float)m_clear.spTex->GetHeight() };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear.spTex, m_clear.pos.x, m_clear.pos.y, size.x * 2, size.y * 2, nullptr, &color);
		}
	}
	
	if (m_gameOverFlg)
	{
		if (m_miss.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_miss.spTex, m_miss.pos.x, m_miss.pos.y);
		}
	}
}

void Result::Init()
{
	if (!m_clear.spTex)
	{
		m_clear.spTex = std::make_shared<KdTexture>();
		m_clear.spTex->Load("Asset/Textures/Text/stageClear.png");
	}
	m_clear.alphaAdd = 0.05f;

	if (!m_miss.spTex)
	{
		m_miss.spTex = std::make_shared<KdTexture>();
		m_miss.spTex->Load("Asset/Textures/Text/miss.png");
	}
	m_miss.pos = { 0, Screen::HalfHeight + (float)m_miss.spTex->GetHeight() };
}

void Result::Reset()
{
	m_miss.pos = { 0, Screen::HalfHeight + (float)m_miss.spTex->GetHeight() };
	m_miss.move = Math::Vector2::Zero;
	m_boundCnt = 0;
	m_gameOverFlg = false;
}
