#include "FlyEnemy.h"

void FlyEnemy::Update()
{
}

void FlyEnemy::PostUpdate()
{
	// アニメーションの更新
// 止まっていたらアニメーションしない
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void FlyEnemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Enemy/FlyEnemy/flyEnemy.gltf");

		// 初期のアニメーション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Fly"));
	}

}
