#pragma once

class BaseScene
{
public :

	BaseScene() {}
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	virtual void Init();

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& obj)
	{
		m_objList.push_back(obj);
	}

	// デバッグ用
	virtual const bool GetDebug() const { return 0; };

protected :

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();

	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;
};
