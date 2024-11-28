#pragma once

#include "../CarryObjectBase.h"

class Box : public CarryObjectBase
{
public:
	Box() {}
	~Box()						override {}

	void Update()				override;
	void PostUpdate()			override;
	void Init()					override;

	void CarryFlg(bool _carryFlg) override;

	void SetParam(Param _param)	override;

	void Reset()	override;

private:
	enum LandSoundType
	{
		Grass,
		Tile,
		Bound,
		MaxNum,
	};

	// 歩く音の配列
	std::weak_ptr<KdSoundInstance3D> m_wpLandSound[LandSoundType::MaxNum];

	// 当たり判定
	void HitJudge();

	// 角度
	float m_degAng	= 0;

	enum Dir
	{
		RightFront,
		RightBack,
		LeftFront,
		LeftBack,
		Up,
		Down,
		Max
	};
	Math::Vector3 m_edgeBasePos[Dir::Max] = {};
	Math::Vector3 m_edgePos[Dir::Max] = {};

	Math::Vector3 m_centerPos;

	// レイを少し上から出す処理をやめるフラグ
	bool m_rayDownFlg	= false;

	// 下に落ちたら、リスポーンさせる
	float m_underLine	= 0;

	Math::Vector3 m_oldPos;

	// 回転床との角度によってレイの順番を変える
	enum class Ray
	{
		Left,
		Right,
		Center,
	};
	Ray m_ray = Ray::Left;

	// 地面に付いた瞬間かどうかを判断するフラグ
	bool m_landFlg = true;

	// ステージが始まった最初の着地音をなくすフラグ
	bool m_firstLandFlg = true;

	// JSONファイルのパス
	std::string_view m_path = "Asset/Data/Json/Terrain/CarryObject/Box/Box.json";

	// JSONのデータをロードする
	void DataLoad();
};