#pragma once

#include "../EventObjectBase.h"

class FinalGoal : public EventObjectBase
{
public:
	FinalGoal()				{}
	~FinalGoal()	override{}

	void Update()	override;
	void GenerateDepthMapFromLight()	override;
	void DrawLit()	override;
	void Init()		override;

	void SetParam(const Param& _param)	override;

	const bool IsFirstClearProcess() const { return m_firstClear == FirstClear::EndProcess; }
	void FirstClearProcess();
	void SetFirstClearFlg(const bool _firstClearFlg);

private:
	// 初クリアしたときに古いモデルから新しいモデルに変わる演出をするための古いモデル
	std::shared_ptr<KdModelData> m_spOldModel = nullptr;

	// 新しいモデルの行列
	Math::Matrix m_newMat;
	// 新しいモデルの座標
	Math::Vector3 m_newPos;

	Math::Vector3 m_pos = Math::Vector3::Zero;

	// スケール
	const float m_defaultScale = 0.5f;

	// SetParamに入ったかどうかのフラグ
	bool m_setParamFlg = false;
	
	enum class FirstClear
	{
		OldModel,
		StartProcess,
		EndProcess,
	};
	FirstClear m_firstClear = FirstClear::OldModel;
	// 初クリアかどうか
	bool m_firstClearFlg = false;

	// 初クリア時の演出用
	float m_scale = 0;					// 拡縮
	float m_addScale = 0.005f;			// 拡縮の加算値
	const float m_addAddScale = 0.005f;	// 拡縮の加算値の加算値(大きくしていく値を大きくする)
	const float m_maxScale = 0.7f;		// 上限の拡縮
	const float m_sumScale = 0.025f;	// 拡縮の減算値
	bool m_sumFlg = false;				// 大きくするかどうか
	bool m_smokeFlg = false;			// 煙を出したかどうか
	int m_stayCount = 0;				// 待機カウント
	const int m_stayTime = 60;			// 待機時間

	// 音
	std::weak_ptr<KdSoundInstance3D> m_wpSound;
	bool m_soundFlg = false;

	int count = 0;
};