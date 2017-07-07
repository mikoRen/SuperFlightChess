#include"Block.h"

Block::Block()
{
	_color = yellow;
	position = Vec2::ZERO;
}

Block::~Block()
{
}

Block* Block::create(color c, Vec2 p)
{
	Block* block = new Block();
	block->_color = c;
	block->position = p;
	switch (c)
	{
	case yellow:
		block->sprite = Sprite::create("yellow.png");
		break;
	case blue:
		block->sprite = Sprite::create("blue.png");
		break;
	case green:
		block->sprite = Sprite::create("green.png");
		break;
	case red:
		block->sprite = Sprite::create("red.png");
		break;
	case black:
		block->sprite = Sprite::create("black.png");
		break;
	case white:
		block->sprite = Sprite::create("white.png");
		break;
	default:
		break;
	}
	return block;
}
