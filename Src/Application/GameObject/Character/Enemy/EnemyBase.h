#pragma once
#include "../CharacterBase.h"

class EnemyBase : public CharacterBase
{
public:
	EnemyBase()							{}
	~EnemyBase()				override{}

	virtual void Update()		override {}
	virtual void PostUpdate()	override {}

	virtual void Init()			override {}

	// ターゲットをセットする
	void SetTarget(const std::shared_ptr<KdGameObject>& _spTarget) { m_wpTarget = _spTarget; }

	// 情報
	struct Info
	{
		Math::Vector3 startPos	= Math::Vector3::Zero;	// 最初の座標
		float moveArea			= 0;					// 行動範囲
		float searchArea		= 0;					// 索敵範囲
	};

	// 情報をセットする
	virtual void SetInfo(Info _info) { m_info = _info; }

protected:
	// ターゲット
	std::weak_ptr<KdGameObject> m_wpTarget;

	// 情報
	Info m_info;
};