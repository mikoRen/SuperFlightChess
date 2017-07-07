#pragma once
#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "cocos2d.h"
USING_NS_CC;

enum color
{
	yellow, blue, green, red, black, white
};

class Block : public cocos2d::Object {
public:
	color _color;
	Vec2 position;
	Sprite* sprite;
	Block();
	~Block();
	static Block* create(color, Vec2);
};


#endif // !_BLOCK_H_
