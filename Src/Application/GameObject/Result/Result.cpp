#include "Result.h"

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
}

void Result::Init()
{
	if (!m_clear.spTex)
	{
		m_clear.spTex = std::make_shared<KdTexture>();
		m_clear.spTex->Load("Asset/Textures/Text/stageClear.png");
	}
	m_clear.alphaAdd = 0.05f;
}