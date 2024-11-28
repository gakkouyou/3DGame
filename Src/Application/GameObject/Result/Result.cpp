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

	if (m_goalFlg == true)
	{
		m_stayCount++;
		if (m_stayCount > m_stayTime)
		{
			// 紙吹雪
			int create = rand();
			if (create % 100 < 60)
			{
				Tex paper;
				float x = rand() % (int)Screen::Width - Screen::Width / 2;
				float y = Screen::Height / 2 + 30.0f;
				float moveX = rand() % 7 - 3.0f;
				float moveY = -(rand() % 5 + 5.0f);
				Math::Color color = { (rand() % 10 + 1) / 10.0f , (rand() % 10 + 1) / 10.0f, (rand() % 10 + 1) / 10.0f, 1 };
				paper.pos = { x, y };
				paper.move = { moveX, moveY };
				paper.angle = rand() % 360;
				paper.color = color;
				m_paper.push_back(paper);
			}

			auto it = m_paper.begin();
			while (it != m_paper.end())
			{
				Math::Vector2 pos = it->pos;
				if (abs(pos.x) > Screen::Width / 2 + 30.0f || pos.y < -Screen::Height / 2 - 30.0f)
				{
					it = m_paper.erase(it);
				}
				else
				{
					it->pos += it->move;
					it->angle += 0.5f;
					it++;
				}
			}
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
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_clear.spTex, (int)m_clear.pos.x, (int)m_clear.pos.y, nullptr, &color);
		}
	}

	if (m_goalFlg == true)
	{
		for (auto& paper : m_paper)
		{
			Math::Rectangle rect = { 0, 0, (long)m_spPaperTex->GetWidth(), (long)m_spPaperTex->GetHeight() };
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPaperTex, paper.pos.x, paper.pos.y, rect.width, rect.height, &rect, &paper.color);
		}
	}
	
	if (m_gameOverFlg)
	{
		if (m_miss.spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_miss.spTex, (int)m_miss.pos.x, (int)m_miss.pos.y);
		}
	}
}

void Result::Init()
{
	m_clear.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Result/stageClear.png");
	m_clear.alphaAdd = 0.05f;

	m_miss.spTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Result/miss.png");
	m_miss.pos = { 0, Screen::HalfHeight + (float)m_miss.spTex->GetHeight() };

	m_spPaperTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Result/paper.png");
}

void Result::Reset()
{
	m_miss.pos = { 0, Screen::HalfHeight + (float)m_miss.spTex->GetHeight() };
	m_miss.move = Math::Vector2::Zero;
	m_boundCnt = 0;
	m_gameOverFlg = false;
}
