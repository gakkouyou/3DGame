﻿#pragma once

class EventObjectBase : public KdGameObject
{
public:
	EventObjectBase() {}
	~EventObjectBase()		override {}

	virtual void Update()	override {}
	virtual void GenerateDepthMapFromLight()	override {}
	virtual void DrawLit()	override {}
	virtual void DrawUnLit()override {}
	virtual void Init()		override;

	struct Param
	{
		Math::Vector3 basePos;	// オブジェクトの始めの座標
		Math::Vector3 secondPos;// 二つ目の座標 オブジェクトによって使い分ける
		int modelNum = 0;		// モデルを使い分けるオブジェクト用
	};

	virtual void SetParam(const Param& _param) { m_param = _param; }

	const Param& GetParam() const { return m_param; }

	virtual void OnHit()	override {}

protected:
	// モデル
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	Param m_param;
};