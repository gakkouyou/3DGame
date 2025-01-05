#pragma once

class MoveObjectRideProcess
{
public:

	// 動く床に乗った時の処理　引数:自分の行列,乗っているオブジェクトの古い行列,乗っているオブジェクトの新しい座標
	Math::Vector3 MoveGroundRide(Math::Matrix& _mWorld, const Math::Matrix& _oldMat, const Math::Vector3& _newPos);

	// 回る床に乗った時の処理
	bool RotationGroundRide(Math::Vector3& _pos, const Math::Vector3& _rideObjectPos, const Math::Vector3& _rotDegAng);

	// プロペラに乗った時の処理
	void PropellerRide(Math::Vector3& _pos, const Math::Vector3& _rideObjectPos, const Math::Vector3& _rotDegAng, float& _degAng);

private:

	MoveObjectRideProcess() {}
	~MoveObjectRideProcess() {}

public:

	// シングルトンパターン
	static MoveObjectRideProcess& Instance()
	{
		static MoveObjectRideProcess instance;
		return instance;
	}
};
