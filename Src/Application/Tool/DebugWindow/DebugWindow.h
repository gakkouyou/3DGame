#pragma once

class ObjectController;

class DebugWindow
{
public:

	void Draw();
	void Init();

	void Release();

	void SetObjectController(const std::shared_ptr<ObjectController>& _spObjectController) { m_wpObjectController = _spObjectController; }

	const Math::Vector3 GetPos() const { return m_pos; }
	void SetPos(const Math::Vector3 _pos) { m_pos = _pos; }

	// 動く系統の情報
	struct MoveObjectInfo
	{
		Math::Vector3 startPos;
		Math::Vector3 goalPos;
		float speed;
		int stayTime;
	};
	// 動く系統の情報ゲット関数
	const MoveObjectInfo GetMoveObjectInfo() const { return m_moveObjectInfo; }

private:
	std::weak_ptr<ObjectController> m_wpObjectController;

	// オブジェクトの座標
	Math::Vector3	m_pos = Math::Vector3::Zero;

	MoveObjectInfo m_moveObjectInfo{ Math::Vector3::Zero, Math::Vector3::Zero, 0, 0 };

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