#pragma once

#include "../EventObjectBase.h"

class CameraBase;

class StageSelectObject : public EventObjectBase
{
public:
	StageSelectObject() {}
	~StageSelectObject()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void DrawSprite()	override;
	void Init()		override;

	void SetParam(const Param& _param)	override;

	void SetCamera(const std::weak_ptr<CameraBase>& _wpCamera) { m_wpCamera = _wpCamera; }

	void OnHit()	override;

private:
	float m_degAng = 0.0f;
	const float m_addDegAng = 0.7f;

	std::shared_ptr<KdModelData> m_spSphereModel = nullptr;

	// 吹き出し
	std::shared_ptr<KdTexture> m_spSpeechBubbleTex = nullptr;
	// ステージの画像
	std::shared_ptr<KdTexture> m_spStageTex = nullptr;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	bool m_onHitFlg = false;

	Math::Matrix m_objMat;
};