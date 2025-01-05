#include "MoveObjectRideProcess.h"

Math::Vector3 MoveObjectRideProcess::MoveGroundRide(Math::Matrix& _mWorld, const Math::Matrix& _oldMat, const Math::Vector3& _newPos)
{
	Math::Matrix localMatFromRideObject = _mWorld * _oldMat.Invert();

	Math::Matrix hitTerrainTransMat = Math::Matrix::CreateTranslation(_newPos);

	_mWorld = localMatFromRideObject * hitTerrainTransMat;

	return _mWorld.Translation();
}

bool MoveObjectRideProcess::RotationGroundRide(Math::Vector3& _pos, const Math::Vector3& _rideObjectPos, const Math::Vector3& _rotDegAng)
{
	// 自分から回転床までの距離
	Math::Vector3 vec = _pos - _rideObjectPos;
	vec.z = 0;

	float length = vec.Length();
	// 移動する前の回転床から見た自分の角度
	float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.y));

	if (degAng > 90 || degAng < -90)
	{
		return true;
	}

	if (degAng <= 90 && degAng >= -90)
	{
		degAng -= 90;
		if (degAng < 0)
		{
			degAng += 360;
		}
		degAng = 360 - degAng;

		degAng += _rotDegAng.z;
		_pos.x = _rideObjectPos.x + length * cos(DirectX::XMConvertToRadians(degAng));
		_pos.y = _rideObjectPos.y + length * sin(DirectX::XMConvertToRadians(degAng));
	}

	return false;
}

void MoveObjectRideProcess::PropellerRide(Math::Vector3& _pos, const Math::Vector3& _rideObjectPos, const Math::Vector3& _rotDegAng, float& _degAng)
{
	// 自分から回転床までの距離
	Math::Vector3 vec = _pos - _rideObjectPos;
	vec.y = 0;

	float length = vec.Length();
	// 移動する前の回転床から見た自分の角度
	float degAng = DirectX::XMConvertToDegrees(atan2(vec.x, vec.z));
	degAng -= 90;
	if (degAng < 0)
	{
		degAng += 360;
	}
	if (degAng >= 360)
	{
		degAng -= 360;
	}
	degAng = 360 - degAng;

	// 回転床が回転する角度
	degAng -= _rotDegAng.y;
	_pos.x = _rideObjectPos.x + length * cos(DirectX::XMConvertToRadians(degAng));
	_pos.z = _rideObjectPos.z + length * sin(DirectX::XMConvertToRadians(degAng));

	// 角度を変える
	_degAng += _rotDegAng.y;
}
