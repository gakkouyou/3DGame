#include "EnemyBase.h"

void EnemyBase::Init()
{
	// 大まかなオブジェクトのタイプ
	m_baseObjectType = BaseObjectType::Enemy;
}

void EnemyBase::Reset()
{
	m_isExpired = true;
}
