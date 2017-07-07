#ifndef _PLANE_H_
#define _PLANE_H_

#include "cocos2d.h"
#include "Block.h"
USING_NS_CC;

enum dir {
	up, down, left, right
};

class plane :public cocos2d::Object {
public:
	plane();
	~plane();
	plane(color _c, Vec2 _position, dir _dir);
	static plane* create(color _c, Vec2 _position, dir _dir);

	Sprite* spr;
	dir direction;
	color c;
	Vec2 position;
	int energy;
};

#endif // !_PLANE_H_