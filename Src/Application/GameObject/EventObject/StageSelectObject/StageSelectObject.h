#pragma once

#include "../EventObjectBase.h"

class CameraBase;
class StageSelectUI;

class StageSelectObject : public EventObjectBase
{
public:
	StageSelectObject() {}
	~StageSelectObject()	override {}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetParam(const Param& _param)	override;

	// カメラをセット
	void SetCamera(const std::weak_ptr<CameraBase>& _wpCamera) { m_wpCamera = _wpCamera; }

	// ステージセレクトのUIをセット
	void SetStageSelectUI(const std::weak_ptr<StageSelectUI>& _wpStageSelectUI) { m_wpStageSelectUI = _wpStageSelectUI; }

	void OnHit()	override;

private:
	float m_degAng = 0;
	const float m_addDegAng = 0.7f;

	// 時計のパーツのモデル
	std::shared_ptr<KdModelData> m_spClockPartsModel = nullptr;
	// パーツの回転角度
	float m_partsDegAng = 0;
	const float m_addPartsDegAng = 0.3f;
	Math::Matrix m_partsMat;

	std::shared_ptr<KdModelData> m_spSphereModel = nullptr;

	// 吹き出し
	std::shared_ptr<KdTexture> m_spSpeechBubbleTex = nullptr;
	// ステージの画像
	std::shared_ptr<KdTexture> m_spStageTex = nullptr;

	// ステージセレクトのUI
	std::weak_ptr<StageSelectUI> m_wpStageSelectUI;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	bool m_onHitFlg = false;

	Math::Matrix m_objMat;

	// 拡縮
	float m_scale = 0;
	const float m_maxScale = 1.5f;
	const float m_minScale = 1.0f;
	const float m_addScale = 0.05f;
};