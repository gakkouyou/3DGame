﻿#include "KdCollision.h"
#include "Application/main.h"
using namespace DirectX;

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// レイの当たり判定
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイの情報を逆行列化する
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// レイとポリゴンを判定する際に全ての頂点を行列移動させるとポリゴン数によって処理コストが変わるため非常に不安定
// レイの情報は1つしかないためレイだけを逆行列化する事で処理の安定化＋1度しか計算が行われないため最大の効率化にもなる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void InvertRayInfo(DirectX::XMVECTOR& rayPosInv, DirectX::XMVECTOR& rayDirInv, float& rayRangeInv, float& scaleInv, 
	const DirectX::XMMATRIX& matrix, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange)
{
	// ターゲットの逆行列でレイを変換
	DirectX::XMMATRIX invMat = XMMatrixInverse(0, matrix);

	// レイの判定開始位置を逆変換
	rayPosInv = XMVector3TransformCoord(rayPos, invMat);

	// レイの方向を逆変換
	rayDirInv = XMVector3TransformNormal(rayDir, invMat);

	// 拡大率を逆変換
	scaleInv = DirectX::XMVector3Length(rayDirInv).m128_f32[0];

	// レイの方向ベクトルの長さ=拡大率で判定限界距離を補正
	// ※逆行列に拡縮が入っていると、レイの長さが変わるため
	// レイが当たった座標からの距離に拡縮が反映されてしまうので
	// 判定用の最大距離にも拡縮を反映させておく
	rayRangeInv = rayRange * scaleInv;

	// 方角レイとして正しく扱うためには長さが１でなければならないので正規化
	rayDirInv = DirectX::XMVector3Normalize(rayDirInv);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイとの当たり判定結果をリザルトにセットする
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void SetRayResult(CollisionMeshResult& result, bool isHit, float closestDist, 
	const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange)
{
	// リザルトに結果を格納
	result.m_hit = isHit;

	result.m_hitPos = DirectX::XMVectorAdd(rayPos, DirectX::XMVectorScale(rayDir, closestDist));

	result.m_hitDir = DirectX::XMVectorScale(rayDir, -1);

	result.m_overlapDistance = rayRange - closestDist;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイ対ポリゴン(KdMesh以外の任意の多角形ポリゴン)の当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool PolygonsIntersect(const KdPolygon& poly, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange,
	const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//--------------------------------------------------------
	// レイの逆行列化
	//--------------------------------------------------------
	DirectX::XMVECTOR rayPosInv, rayDirInv;
	float rayRangeInv = 0;
	float scaleInv = 0;

	InvertRayInfo(rayPosInv, rayDirInv, rayRangeInv, scaleInv,
		matrix, rayPos, rayDir, rayRange);

	//--------------------------------------------------------
	// レイ vs 全ての面
	//--------------------------------------------------------

	// ヒット判定
	bool isHit = false;
	float closestDist = FLT_MAX;

	// 頂点リスト取得
	std::vector<Math::Vector3> positions;
	poly.GetPositions(positions);
	size_t faceNum = positions.size() - 2;

	// 全ての面(三角形)
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// レイと三角形の判定
		float hitDist = FLT_MAX;
		if (!DirectX::TriangleTests::Intersects(rayPosInv, rayDirInv,
			positions[faceIdx], positions[faceIdx + 1], positions[faceIdx + 2],
			hitDist))
		{
			continue;
		}

		// レイの判定範囲外なら無視
		if (hitDist > rayRangeInv) { continue; }

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult) { return isHit; }

		// 最短距離の更新判定処理
		closestDist = std::min(hitDist, closestDist);

		isHit = true;
	}

	if (pResult && isHit)
	{
		SetRayResult(*pResult, isHit, closestDist / scaleInv, rayPos, rayDir, rayRange);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイ対メッシュの当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool MeshIntersect(const KdMesh& mesh, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir,
	float rayRange, const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//--------------------------------------------------------
	// ブロードフェイズ
	// 　比較的軽量なAABB vs レイな判定で、
	// 　あきらかにヒットしない場合は、これ以降の判定を中止
	//--------------------------------------------------------
	{
		// AABB vs レイ
		float AABBdist = FLT_MAX;
		DirectX::BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		if (aabb.Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		// 最大距離外なら範囲外なので中止
		if (AABBdist > rayRange) { return false; }
	}

	//--------------------------------------------------------
	// レイの逆行列化
	//--------------------------------------------------------
	DirectX::XMVECTOR rayPosInv, rayDirInv;
	float rayRangeInv = 0;
	float scaleInv = 0;

	InvertRayInfo(rayPosInv, rayDirInv, rayRangeInv, scaleInv,
		matrix, rayPos, rayDir, rayRange);

	//--------------------------------------------------------
	// ナローフェイズ
	// 　レイ vs 全ての面
	//--------------------------------------------------------

	// ヒット判定
	bool isHit = false;
	float closestDist = FLT_MAX;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const KdMeshFace* pFaces = &mesh.GetFaces()[0];
	auto& vertices = mesh.GetVertexPositions();
	UINT faceNum = mesh.GetFaces().size();

	// 全ての面(三角形)
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// レイと三角形の判定
		float hitDist = FLT_MAX;
		if (!DirectX::TriangleTests::Intersects(rayPosInv, rayDirInv,
			vertices[idx[0]], vertices[idx[1]], vertices[idx[2]],
			hitDist))
		{
			continue;
		}

		// レイの判定範囲外なら無視
		if (hitDist > rayRangeInv) { continue; }
	
		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult) { return isHit; }

		// 最短距離の更新判定処理
		closestDist = std::min(hitDist, closestDist);

		isHit = true;
	}

	if (pResult && isHit)
	{
		SetRayResult(*pResult, isHit, closestDist / scaleInv, rayPos, rayDir, rayRange);
	}

	return isHit;
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// スフィアの当たり判定
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィアの情報を逆行列化する
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// レイと同様の理由
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void InvertSphereInfo(DirectX::XMVECTOR& spherePosInv, DirectX::XMVECTOR& sphereScale, float& radiusSqr,
	const DirectX::XMMATRIX& matrix, const DirectX::BoundingSphere& sphere)
{
	// メッシュの逆行列で、球の中心座標を変換(メッシュの座標系へ変換される)
	DirectX::XMMATRIX invMat = XMMatrixInverse(0, matrix);
	spherePosInv = XMVector3TransformCoord(XMLoadFloat3(&sphere.Center), invMat);

	// 半径の二乗(判定の高速化用)
	radiusSqr = sphere.Radius * sphere.Radius;	// 半径の２乗

	// 行列の各軸の拡大率を取得しておく
	sphereScale.m128_f32[0] = DirectX::XMVector3Length(matrix.r[0]).m128_f32[0];
	sphereScale.m128_f32[1] = DirectX::XMVector3Length(matrix.r[1]).m128_f32[0];
	sphereScale.m128_f32[2] = DirectX::XMVector3Length(matrix.r[2]).m128_f32[0];
	sphereScale.m128_f32[3] = 0;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィアとポリゴンの最近接点を元に接触しているかどうかを判定
// 次のポリゴンの判定の間に当たらない位置までスフィアを移動させる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static bool HitCheckAndPosUpdate(DirectX::XMVECTOR& finalPos, DirectX::XMVECTOR& finalHitPos, 
	const DirectX::XMVECTOR& nearPoint, const DirectX::XMVECTOR& objScale, float radiusSqr, float sphereRadius)
{
	// 最近接点→球の中心　ベクトルを求める
	DirectX::XMVECTOR vToCenter = finalPos - nearPoint;

	// XYZ軸が別々の大きさで拡縮されてる状態の場合、球が変形してる状態なため正確な半径がわからない。
	// そこでscaleをかけてXYZ軸のスケールが均等な座標系へ変換する
	vToCenter *= objScale;

	// 最近接点が半径の2乗より遠い場合は、衝突していない
	if (DirectX::XMVector3LengthSq(vToCenter).m128_f32[0] > radiusSqr)
	{
		return false;
	}

	// 押し戻し計算
	// めり込んでいるぶんのベクトルを計算
	DirectX::XMVECTOR vPush = DirectX::XMVector3Normalize(vToCenter);

	vPush *= sphereRadius - DirectX::XMVector3Length(vToCenter).m128_f32[0];

	// 拡縮を考慮した座標系へ戻す
	vPush /= objScale;

	// 球の中心座標を更新
	finalPos += vPush;

	finalHitPos = nearPoint;

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィアとの当たり判定結果をリザルトにセットする
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void SetSphereResult(CollisionMeshResult& result, bool isHit, const DirectX::XMVECTOR& hitPos,
	const DirectX::XMVECTOR& finalPos, const DirectX::XMVECTOR& beginPos, const std::vector<DirectX::XMVECTOR> normal = {})
{
	result.m_hit = isHit;

	result.m_hitPos = hitPos;

	result.m_hitDir = DirectX::XMVectorSubtract(finalPos, beginPos);

	result.m_overlapDistance = DirectX::XMVector3Length(result.m_hitDir).m128_f32[0];

	result.m_hitDir = DirectX::XMVector3Normalize(result.m_hitDir);

	result.m_hitNormalList = normal;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィア対ポリゴン(KdMesh以外の任意の多角形ポリゴン)の当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool PolygonsIntersect(const KdPolygon& poly, const DirectX::BoundingSphere& sphere, const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//------------------------------------------
	// 球とポリゴンとの詳細判定
	//------------------------------------------
	// １つでもヒットしたらtrue
	bool isHit = false;

	// 頂点リスト取得
	std::vector<Math::Vector3> positions;
	poly.GetPositions(positions);
	size_t faceNum = positions.size() - 2;

	DirectX::XMVECTOR finalHitPos = {};	// 当たった座標の中でも最後の座標
	DirectX::XMVECTOR finalPos = {};	// 各面に押されて最終的に到達する座標：判定する球の中心
	DirectX::XMVECTOR objScale = {};	// ターゲットオブジェクトの各軸の拡大率
	float radiusSqr = 0.0f;
	InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, sphere);

	// 全ての面と判定
	// ※判定はポリゴンのローカル空間で行われる
	for (UINT faceIndx = 0; faceIndx < faceNum; faceIndx++)
	{
		DirectX::XMVECTOR nearPoint;

		// 点 と 三角形 の最近接点を求める
		KdPointToTriangle(finalPos,
			positions[faceIndx],
			positions[faceIndx + 1],
			positions[faceIndx + 2],
			nearPoint);

		// 当たっているかどうかの判定と最終座標の更新
		isHit |= HitCheckAndPosUpdate(finalPos, finalHitPos, nearPoint, objScale, radiusSqr, sphere.Radius);

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult && isHit) { return isHit; }
	}

	// リザルトに結果を格納
	if (pResult && isHit)
	{
		SetSphereResult(*pResult, isHit, XMVector3TransformCoord(finalHitPos, matrix),
			XMVector3TransformCoord(finalPos, matrix), XMLoadFloat3(&sphere.Center));
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィア対メッシュの当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool MeshIntersect(const KdMesh& mesh, const DirectX::BoundingSphere& sphere,
	const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//------------------------------------------
	// ブロードフェイズ
	// 　高速化のため、まずは境界ボックス(AABB)で判定
	// 　この段階でヒットしていないなら、詳細な判定をする必要なし
	//------------------------------------------
	{
		// メッシュのAABBを元に、行列で変換したAABBを作成
		DirectX::BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		if (aabb.Intersects(sphere) == false) { return false; }
	}

	//------------------------------------------
	// ナローフェイズ
	// 　球とメッシュとの詳細判定
	//------------------------------------------

	// １つでもヒットしたらtrue
	bool isHit = false;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const auto* pFaces = &mesh.GetFaces()[0];
	UINT faceNum = mesh.GetFaces().size();
	auto& vertices = mesh.GetVertexPositions();

	DirectX::XMVECTOR finalHitPos = {};	// 当たった座標の中でも最後の座標
	DirectX::XMVECTOR finalPos = {};	// 各面に押されて最終的に到達する座標：判定する球の中心
	DirectX::XMVECTOR objScale = {};	// ターゲットオブジェクトの各軸の拡大率
	float radiusSqr = 0.0f;
	InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, sphere);

	// 全ての面と判定
	// ※判定はメッシュのローカル空間で行われる
	std::vector<std::vector<Math::Vector3>> vList;
	for (UINT faceIdx = 0; faceIdx < faceNum; faceIdx++)
	{
		DirectX::XMVECTOR nearPoint;

		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// 点 と 三角形 の最近接点を求める
		KdPointToTriangle(finalPos, vertices[idx[0]], vertices[idx[1]], vertices[idx[2]], nearPoint);

		// 面を形成する頂点座標をvに格納
		if (HitCheckAndPosUpdate(finalPos, finalHitPos, nearPoint, objScale, radiusSqr, sphere.Radius))
		{
			std::vector<Math::Vector3> tmp = { vertices[idx[0]], vertices[idx[1]], vertices[idx[2]] };
			vList.push_back(tmp);
			isHit = true;
		}

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult && isHit) { return isHit; }
	}

	// リザルトに結果を格納
	if (pResult && isHit)
	{
		std::vector<DirectX::XMVECTOR> normalList;

		for (auto& v : vList)
		{
			// 面の法線を求める
			// 3頂点のワールド座標を算出
			Math::Vector3 a = XMVector3TransformCoord(v[0], matrix);
			Math::Vector3 b = XMVector3TransformCoord(v[1], matrix);
			Math::Vector3 c = XMVector3TransformCoord(v[2], matrix);

			// ベクトル作成
			Math::Vector3 ab = b - a;
			ab.Normalize();
			Math::Vector3 ac = c - a;
			ac.Normalize();

			// 外積を求める = 法線
			Math::Vector3 n;
			n = ab.Cross(ac);
			n.Normalize();
			normalList.push_back(n);
		}

		SetSphereResult(*pResult, isHit, XMVector3TransformCoord(finalHitPos, matrix),
			XMVector3TransformCoord(finalPos, matrix), XMLoadFloat3(&sphere.Center), normalList);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 点 vs 面を形成する三角形との最近接点を求める
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdPointToTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c, DirectX::XMVECTOR& outPt)
{
	// ※参考:[書籍]「ゲームプログラミングのためのリアルタイム衝突判定」

	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;

	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ac.Dot(ap);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		outPt = a;	// 重心座標(1,0,0)
		return;
	}

	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ac.Dot(bp);

	if (d3 >= 0.0f && d4 <= d3)
	{
		outPt = b;	// 重心座標(0,1,0)
		return;
	}

	// pがabの辺領域の中にあるかどうかチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;

	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		outPt = a + ab * v;	// 重心座標(1-v,v,0)
		return;
	}

	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];;//ac.Dot(cp);

	if (d6 >= 0.0f && d5 <= d6)
	{
		outPt = c;	// 重心座標(0,0,1)
		return;
	}

	// pがacの辺領域の中にあるかどうかチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;

	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		outPt = a + ac * w;	// 重心座標(1-w,0,w)
		return;
	}

	// pがbcの辺領域の中にあるかどうかチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;

	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		outPt = b + (c - b) * w;	// 重心座標(0,1-w,w)
		return;
	}

	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPt = a + ab * v + ac * w;	// = u*a + v*b + w*c, u = va*demon = 1.0f - v - w
}













// 追加部分

void KdPointToTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c, Math::Vector3& outPt)
{
	// ※参考:[書籍]「ゲームプログラミングのためのリアルタイム衝突判定」

	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;

	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ac.Dot(ap);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		outPt = a;	// 重心座標(1,0,0)
		return;
	}

	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ac.Dot(bp);

	if (d3 >= 0.0f && d4 <= d3)
	{
		outPt = b;	// 重心座標(0,1,0)
		return;
	}

	// pがabの辺領域の中にあるかどうかチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;

	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		outPt = a + ab * v;	// 重心座標(1-v,v,0)
		return;
	}

	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];;//ac.Dot(cp);

	if (d6 >= 0.0f && d5 <= d6)
	{
		outPt = c;	// 重心座標(0,0,1)
		return;
	}

	// pがacの辺領域の中にあるかどうかチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;

	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		outPt = a + ac * w;	// 重心座標(1-w,0,w)
		return;
	}

	// pがbcの辺領域の中にあるかどうかチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;

	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		outPt = b + (c - b) * w;	// 重心座標(0,1-w,w)
		return;
	}

	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPt = a + ab * v + ac * w;	// = u*a + v*b + w*c, u = va*demon = 1.0f - v - w
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ボックスとの当たり判定結果をリザルトにセットする
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void SetBoxResult(CollisionMeshResult& result, bool isHit, Math::Vector3 hitPos,
	const Math::Vector3& finalPos, const Math::Vector3)
{
	result.m_hit = isHit;

	Math::Vector3 dir = finalPos - hitPos;

	result.m_hitDir = dir;

	result.m_hitPos = hitPos;

	result.m_overlapDistance = (dir).Length();

	result.m_hitDir = DirectX::XMVector3Normalize(result.m_hitDir);
}

// ボックスと三角形の当たり判定
static bool HitCheck(const Math::Vector3 center, const float left, const float right, const float up, const float down, const float front, const float back, Math::Vector3 a, Math::Vector3 b, Math::Vector3 c, Math::Vector3& pos)
{
	bool isHit = false;
	float length = 0.0f;
	if (a.x > left && a.x < right && a.y > down && a.y < up && a.z > back && a.z < front)
	{
		isHit = true;
		length = (a - center).LengthSquared();
		pos = a;
		return true;
	}

	if (b.x > left && b.x < right && b.y > down && b.y < up && b.z > back && b.z < front)
	{
		isHit = true;
		float tmp = (b - center).LengthSquared();
		if (tmp < length)
		{
			length = tmp;
			pos = b;
		}
	}

	if (c.x > left && c.x < right && c.y > down && c.y < up && c.z > back && c.z < front)
	{
		isHit = true;
		float tmp = (c - center).LengthSquared();
		if (tmp < length)
		{
			pos = c;
		}
	}

	return isHit;
}

// 当たり判定２
static bool HitCheck(Math::Vector3& finalPos, Math::Vector3& finalHitPos,
	Math::Vector3& nearPoint, const Math::Vector3& center, const float left, const float right, const float up, const float down, const float front, const float back)
{
	// ボックスの外なら当たっていない
	if (nearPoint.x < left || nearPoint.x > right || nearPoint.y < down || nearPoint.y > up || nearPoint.z < back || nearPoint.z > front)
	{
		return false;
	}

	float adjust = 0;

	// 左側にある場合
	if (nearPoint.x < center.x)
	{
		adjust = nearPoint.x - left;
		finalPos.x += adjust;
	}
	// 右側にある場合
	else
	{
		adjust = nearPoint.x - right;
		finalPos.x += adjust;
	}
	
	// 後ろ側にある場合
	if (nearPoint.z < center.z)
	{
		adjust += nearPoint.z - back;
		finalPos.z += adjust;
	}
	// 前側にある場合
	else
	{
		adjust += nearPoint.z - front;
		finalPos.z += adjust;
	}

	finalHitPos = nearPoint;

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ボックス対メッシュの当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool MeshIntersect(const KdMesh& mesh, const DirectX::BoundingBox& box, const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//------------------------------------------
	// ブロードフェイズ
	// 　高速化のため、まずは境界ボックス(AABB)で判定
	// 　この段階でヒットしていないなら、詳細な判定をする必要なし
	//------------------------------------------
	{
		// メッシュのAABBを元に、行列で変換したAABBを作成
		DirectX::BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		if (aabb.Intersects(box) == false) { return false; }
	}

	//------------------------------------------
	// ナローフェイズ
	// 　球とメッシュとの詳細判定
	//------------------------------------------

	// １つでもヒットしたらtrue
	bool isHit = false;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const auto* pFaces = &mesh.GetFaces()[0];
	UINT faceNum = mesh.GetFaces().size();
	auto& vertices = mesh.GetVertexPositions();

	//DirectX::XMVECTOR finalHitPos = {};	// 当たった座標の中でも最後の座標
	Math::Vector3 finalHitPos;
	//DirectX::XMVECTOR finalPos = {};	// 各面に押されて最終的に到達する座標：判定する球の中心
	Math::Vector3 finalPos;
	//DirectX::XMVECTOR objScale = {};	// ターゲットオブジェクトの各軸の拡大率
	//float radiusSqr = 0.0f;
	//InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, box);

	// boxの各端っこの座標
	Math::Vector3 center = box.Center;			// boxの中心
	Math::Vector3 size = box.Extents;			// boxの大きさ

	finalPos = center;

	float left = center.x - size.x;	// 左端
	float right = center.x + size.x;// 右端

	float up = center.y + size.y;	// 上端
	float down = center.y - size.y;	// 下端

	float front = center.z + size.z;// 前面(奥)
	float back = center.z - size.z;	// 後ろ(手前)

	// 当たった頂点の座標
	Math::Vector3 pos;

	// 全ての面と判定
	// ※判定はメッシュのローカル空間で行われる
	for (UINT faceIdx = 0; faceIdx < faceNum; faceIdx++)
	{
		//DirectX::XMVECTOR nearPoint;
		Math::Vector3 nearPoint;

		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		//if (box.Intersects(vertices[idx[0]], vertices[idx[1]], vertices[idx[2]]))
		//{
		//	Application::Instance().m_log.AddLog("true");
		//}

		// ローカル座標からワールド座標に変換
		Math::Vector3 a = XMVector3TransformCoord(vertices[idx[0]], matrix);
		Math::Vector3 b = XMVector3TransformCoord(vertices[idx[1]], matrix);
		Math::Vector3 c = XMVector3TransformCoord(vertices[idx[2]], matrix);

		// 一番近い点を見つける
		KdPointToTriangle(finalPos, a, b, c, nearPoint);

		// 当たったかどうかをチェックする
		//if (HitCheck(center, left, right, up, down, front, back, a, b, c, pos))
		if(HitCheck(finalPos, finalHitPos, nearPoint, center, left, right, up, down, front, back))
		{
			//Application::Instance().m_log.AddLog("true");
			pos = nearPoint;
			isHit = true;
		}
		
		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult && isHit) { return isHit; }

		// 端の座標を再計算
		left	= finalPos.x - size.x;	// 左端
		right	= finalPos.x + size.x;	// 右端

		up		= finalPos.y + size.y;	// 上端
		down	= finalPos.y - size.y;	// 下端

		front	= finalPos.z + size.z;	// 前面(奥)
		back	= finalPos.z - size.z;	// 後ろ(手前)

		// もし当たっていたらboxをずらす
		if (isHit)
		{
			float x = center.x - pos.x;
			if (x > 0)
			{
				// boxの左辺から当たった頂点までの距離
				x = size.x - x;
				// ずらす
				center.x += x;
			}
			else if (x < 0)
			{
				// boxの右辺から当たった頂点までの距離
				x *= -1;
				x = size.x - x;
				// ずらす
				center.x -= x;
			}

			float z = center.z - pos.z;
			if (z > 0)
			{
				// boxの左辺から当たった頂点までの距離
				z = size.x - z;
				// ずらす
				center.z += z;
			}
			else if (z < 0)
			{
				// boxの右辺から当たった頂点までの距離
				z *= -1;
				z = size.z - z;
				// ずらす
				center.x -= z;
			}
		}
	}

	// リザルトに結果を格納
	if (pResult && isHit)
	{	
		SetBoxResult(*pResult, isHit, pos,
			finalPos, center);
	}

	return isHit;
}

// 視錐台カリング用
bool MeshIntersect(const KdMesh& mesh, const DirectX::BoundingFrustum& frustum, const DirectX::XMMATRIX& matrix)
{
	// メッシュのAABBを元に、行列で変換したAABBを作成
	DirectX::BoundingBox aabb;
	mesh.GetBoundingBox().Transform(aabb, matrix);

	if (frustum.Contains(aabb))
	{
		return true;
	}

	return false;
}
