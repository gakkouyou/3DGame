#pragma once

class ObjectController;

class DebugWindow
{
public:

	void Draw();
	void Init();

	void Release();

	void SetObjectController(const std::shared_ptr<ObjectController>& _spObjectController) { m_wpObjectController = _spObjectController; }

	// 動く系統の情報
	struct ObjectInfo
	{
		Math::Vector3 startPos	= Math::Vector3::Zero;
		Math::Vector3 goalPos	= Math::Vector3::Zero;
		float speed				= 0;
		int stayTime			= 0;
		Math::Vector3 degAng	= Math::Vector3::Zero;
	};
	// 動く系統の情報ゲット関数
	const ObjectInfo GetObjectInfo() const { return m_objectInfo; }
	// 動く系統の情報セット関数
	void SetObjectInfo(ObjectInfo _info) { m_objectInfo = _info; }

private:
	std::weak_ptr<ObjectController> m_wpObjectController;

	ObjectInfo m_objectInfo;

private:

	DebugWindow() { Init(); }
	~DebugWindow() {}

public:
	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static DebugWindow& Instance()
	{
		static DebugWindow instance;
		return instance;
	}
};