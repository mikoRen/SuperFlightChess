#include "Plane.h"

plane::plane() {
	c = yellow;
	position = Vec2::ZERO;
	direction = up;
	energy = 0;
}

plane::plane(color _c, Vec2 _position, dir _dir) {
	if (_c == red) {
		spr = Sprite::create("rPlane.png");
	}
	if (_c == blue) {
		spr = Sprite::create("bPlane.png");
	}
	switch (_dir)
	{
	case up:
		spr->setRotation(0);
		break;
	case down:
		spr->setRotation(180);
		break;
	case left:
		spr->setRotation(270);
		break;
	case right:
		spr->setRotation(90);
		break;
	default:
		break;
	}
	c = _c;
	position = _position;
	direction = _dir;
	energy = 0;
}

plane * plane::create(color _c, Vec2 _position, dir _dir)
{
	plane* p = new plane(_c, _position, _dir);
	return p;
}

plane::~plane() {
}