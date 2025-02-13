#include "Lab6/CGameObject.h"
#include "stdafx.h"

#define DTIME 1

void CGameObject::move() {
	_position += _velocity * DTIME;
}

bool CGameObject::collide(CGameObject& obj) {
	cv::Point3f objPos = obj.get_pos();
	if (abs(_position.x - objPos.x) <= _radius &&
		abs(_position.y - objPos.y) <= _radius &&
		abs(_position.z - objPos.z) <= _radius)
		return true;

	return false;
}

bool CGameObject::collide_wall(cv::Size board) {
	if (_position.x <= 0 || _position.x >= board.width ||
		_position.y <= 0 || _position.x >= board.height)
		return true;

	return false;
}

void CGameObject::hit() {
	_lives--;
}