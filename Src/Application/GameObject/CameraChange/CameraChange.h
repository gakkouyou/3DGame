#pragma once

class CameraChange : public KdGameObject
{
public:
	CameraChange() {};
	~CameraChange()	override {};

	void Update()	override;
	void Init()		override;

	struct Param
	{
		Math::Vector3	pos;		// 座標
		float			area;		// 範囲
		Math::Vector3	targetPos;	// ターゲットの座標
	};

	virtual Math::Vector3 GetPos() const { return m_param.targetPos; }

	const Param& GetParam() const { return m_param; }
	void SetParam(Param& _param);

	void WriteDebugWire();

private:
	Param m_param;

	bool m_cameraChangeModeFlg = false;
};