#include "GameScene.h"
USING_NS_CC;
using namespace CocosDenshion;

Scene* GameScene::createScene()
{
	auto scene = Scene::create();

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//添加背景
	addBackground();

	//添加点击监听
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	//棋盘初始化
	chessBoardInit();

	//玩家飞机初始化
	playerInit_1();
	playerInit_2();

	//按键初始化
	buttonInit();

	scheduleUpdate();
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("music/BGM.mp3", true);

	return true;
}

void GameScene::update(float f)
{
	if (state == _await) {
		indicator->setString("ROLL");
	}
	else if (state == plane_choose) {
		indicator->setString("CHOOSE");
	}
	else if (state == dir_choose) {
		indicator->setString("DIRECTION");
	}

	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume() - 0.8f);
	char num[5];
	if (turn == red) {
		indicator_red->setVisible(true);
		indicator_blue->setVisible(false);
		if (chosen_plane != nullptr) {
			if (chosen_plane->position == Vec2(11, 11)*BlockSize || chosen_plane->position == Vec2(11, 12)*BlockSize || chosen_plane->position == Vec2(12, 11)*BlockSize || chosen_plane->position == Vec2(12, 12)*BlockSize) {
				player1_skill_indicator->setVisible(true);
			}
		}
	}
	if (turn == blue) {
		indicator_red->setVisible(false);
		indicator_blue->setVisible(true);
	}
	if (dice_point == 0) {
		dice_point_indicator->setVisible(false);
	}
	else {
		dice_point_indicator->setVisible(true);
		sprintf(num, "%d", dice_point);
		dice_point_indicator->setString(num);
	}
	if (chosen_plane != nullptr) {
		chosen_plane->position = chosen_plane->spr->getPosition();
	}
	if (isGameOver()) {
		state = over;
		auto gameOver = Label::createWithTTF("1", "fonts/arial.ttf", 100);
		gameOver->setPosition(Director::getInstance()->getVisibleSize() / 2);
		if (player1.empty()) {
			gameOver->setString("Blue Team Win!");
		}
		else {
			gameOver->setString("Red Team Win!");
		}
		addChild(gameOver, 2);
	}
}

void GameScene::updateCustom(float dt)
{
	upArrow->setVisible(false);
	downArrow->setVisible(false);
	leftArrow->setVisible(false);
	rightArrow->setVisible(false);
	if (chosen_plane->energy == 1) {
		if (turn == red) {
			Vector<plane*> plane_erase;
			//摧毁己方飞机
			for (int i = 0; i < player1.size(); i++) {
				if (player1.at(i) != chosen_plane && player1.at(i)->position == chosen_plane->position) {
					Boom(player1.at(i));
					crush(player1.at(i));
					plane_erase.pushBack(player1.at(i));
					auto director = Director::getInstance();
					director->getRunningScene()->runAction(Shake::create(0.1f, 10));
					player1_kill = true;
				}
			}
			for (int i = 0; i < plane_erase.size(); i++) {
				player1.eraseObject(plane_erase.at(i), false);
			}

			plane_erase.clear();
			//摧毁敌方飞机
			for (int i = 0; i < player2.size(); i++) {
				if (player2.at(i)->position == chosen_plane->position) {
					Boom(player2.at(i));
					crush(player2.at(i));
					plane_erase.pushBack(player2.at(i));
					auto director = Director::getInstance();
					director->getRunningScene()->runAction(Shake::create(0.1f, 10));
					player1_kill = true;
				}
			}
			for (int i = 0; i < plane_erase.size(); i++) {
				player2.eraseObject(plane_erase.at(i), false);
			}

			if (dice_point > 0 && getChoice(chosen_plane) == 1) {
				setDirection(chosen_plane, getOnlyDirection(chosen_plane));
				planeMove(chosen_plane);
			}
			else if (dice_point == 0) {
				if (player1_kill == false) {
					turn = blue;
					chosen_plane->energy = 0;
					chosen_plane = nullptr;
					player1_skill_indicator->stopAllActions();
					player1_skill_indicator->setVisible(false);
				}
				state = _await;
				unschedule(schedule_selector(GameScene::updateCustom));
				return;
			}
			else if (getChoice(chosen_plane) != 1) {
				state = dir_choose;
				arrowUpdate();
				unschedule(schedule_selector(GameScene::updateCustom));
			}
		}
		if (turn == blue) {
			if (getChoice(chosen_plane) == 1 && dice_point > 0) {
				setDirection(chosen_plane, getOnlyDirection(chosen_plane));
				planeMove(chosen_plane);
			}
			else if (dice_point == 0) {
				auto flash = Sprite::create("flash.jpg");
				flash->setScale(3);
				flash->setAnchorPoint(Vec2(0.5, 0));
				switch (chosen_plane->direction)
				{
				case up:
					flash->setRotation(0);
					break;
				case down:
					flash->setRotation(180);
					break;
				case left:
					flash->setRotation(270);
					break;
				case right:
					flash->setRotation(90);
					break;
				default:
					break;
				}
				flash->setPosition(chosen_plane->position);
				addChild(flash, 0);
				flash->runAction(FadeOut::create(1));

				Vector<plane*> plane_erase;
				for (int i = 0; i < player1.size(); i++) {
					if (player1.at(i)->position.x == chosen_plane->position.x) {
						switch (chosen_plane->direction)
						{
						case up:
							if (player1.at(i)->position.y > chosen_plane->position.y) {
								plane_erase.pushBack(player1.at(i));
							}
							break;
						case down:
							if (player1.at(i)->position.y < chosen_plane->position.y) {
								plane_erase.pushBack(player1.at(i));
							}
							break;
						default:
							break;
						}
					}
					else if (player1.at(i)->position.y == chosen_plane->position.y) {
						switch (chosen_plane->direction)
						{
						case left:
							if (player1.at(i)->position.x < chosen_plane->position.x) {
								plane_erase.pushBack(player1.at(i));
							}
							break;
						case right:
							if (player1.at(i)->position.x > chosen_plane->position.x) {
								plane_erase.pushBack(player1.at(i));
							}
							break;
						default:
							break;
						}
					}
				}
				for (int i = 0; i < plane_erase.size(); i++) {
					Boom(plane_erase.at(i));
					crush(plane_erase.at(i));
					player1.eraseObject(plane_erase.at(i));
				}
				plane_erase.clear();
				for (int i = 0; i < player2.size(); i++) {
					if (player2.at(i) != chosen_plane && player2.at(i)->position.x == chosen_plane->position.x) {
						switch (chosen_plane->direction)
						{
						case up:
							if (player2.at(i)->position.y > chosen_plane->position.y) {
								plane_erase.pushBack(player2.at(i));
							}
							break;
						case down:
							if (player2.at(i)->position.y < chosen_plane->position.y) {
								plane_erase.pushBack(player2.at(i));
							}
							break;
						default:
							break;
						}
					}
					else if (player2.at(i) != chosen_plane && player2.at(i)->position.y == chosen_plane->position.y) {
						switch (chosen_plane->direction)
						{
						case left:
							if (player2.at(i)->position.x < chosen_plane->position.x) {
								plane_erase.pushBack(player2.at(i));
							}
							break;
						case right:
							if (player2.at(i)->position.x > chosen_plane->position.x) {
								plane_erase.pushBack(player2.at(i));
							}
							break;
						default:
							break;
						}
					}
				}
				for (int i = 0; i < plane_erase.size(); i++) {
					Boom(plane_erase.at(i));
					crush(plane_erase.at(i));
					player2.eraseObject(plane_erase.at(i));
				}
				player2_skill_indicator->stopAllActions();
				player2_skill_indicator->setVisible(false);
				chosen_plane->energy = 0;
				turn = red;
				state = _await;
				chosen_plane = nullptr;
				unschedule(schedule_selector(GameScene::updateCustom));
			}
			else if (getChoice(chosen_plane) != 1) {
				state = dir_choose;
				arrowUpdate();
				unschedule(schedule_selector(GameScene::updateCustom));
			}
		}
	}
	else {
		if (getChoice(chosen_plane) == 1 && dice_point > 0) {
			setDirection(chosen_plane, getOnlyDirection(chosen_plane));
			planeMove(chosen_plane);
		}
		else if (dice_point == 0) {
			if (turn == red) {
				turn = blue;
			}
			else {
				turn = red;
			}
			state = _await;
			chosen_plane = nullptr;
			unschedule(schedule_selector(GameScene::updateCustom));
		}
		else if (getChoice(chosen_plane) != 1) {
			state = dir_choose;
			arrowUpdate();
			unschedule(schedule_selector(GameScene::updateCustom));
		}
	}
}

bool GameScene::onTouchBegan(Touch * touch, Event * unused_event)
{
	auto touch_point = touch->getLocation();
	if (turn == red) {
		if (state == _await) {
			if (player1_skill_indicator->isVisible() && player1_skill_indicator->getBoundingBox().containsPoint(touch_point)) {
				player1_skill_activated = true;
				auto fade = Sequence::create(FadeOut::create(0.25), FadeIn::create(0.25), nullptr);
				auto repeat_fade = RepeatForever::create(fade);
				player1_skill_indicator->runAction(repeat_fade);
			}
		}
		if (state == plane_choose) {
			for (auto p : player1) {
				if (p->spr->getBoundingBox().containsPoint(touch_point)) {
					if (!isChessBoardContain(p->position)) {
						p->position = Vec2(5, 5)*BlockSize;
						p->spr->setPosition(Vec2(5, 5)*BlockSize);
						dice_point = 0;
						turn = blue;
						state = _await;
					}
					else {
						chosen_plane = p;
						if (player1_skill_activated == true) {
							chosen_plane->energy = 1;
							player1_skill_activated = false;
						}
					}
					break;
				}
			}
			if (chosen_plane != nullptr) {
				schedule(schedule_selector(GameScene::updateCustom), 0.40);
			}
		}
		if (state == dir_choose) {
			bool isChose = 0;
			if (upArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, up);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (downArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, down);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (leftArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, left);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (rightArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, right);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
		}
	}
	if (turn == blue) {
		if (state == _await) {
			if (player2_skill_indicator->isVisible() && player2_skill_indicator->getBoundingBox().containsPoint(touch_point)) {
				player2_skill_activated = true;
				auto fade = Sequence::create(FadeOut::create(0.25), FadeIn::create(0.25), nullptr);
				auto repeat_fade = RepeatForever::create(fade);
				player2_skill_indicator->runAction(repeat_fade);
			}
		}
		if (state == plane_choose) {
			for (auto p : player2) {
				if (p->spr->getBoundingBox().containsPoint(touch_point)) {
					if (!isChessBoardContain(p->position)) {
						p->spr->setPosition(Vec2(18, 18)*BlockSize);
						p->position = Vec2(18, 18)*BlockSize;
						turn = red;
						dice_point = 0;
						state = _await;
					}
					else {
						chosen_plane = p;
						if (player2_skill_activated == true) {
							chosen_plane->energy = 1;
							player2_skill_activated = false;
						}
					}
					break;
				}
			}
			if (chosen_plane != nullptr) {
				schedule(schedule_selector(GameScene::updateCustom), 0.40);
			}
		}
		if (state == dir_choose) {
			if (upArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, up);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (downArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, down);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (leftArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, left);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
			else if (rightArrow->getBoundingBox().containsPoint(touch_point)) {
				setDirection(chosen_plane, right);
				planeMove(chosen_plane);
				schedule(schedule_selector(GameScene::updateCustom), 0.40, kRepeatForever, 0.40);
			}
		}
	}
	return true;
}

int GameScene::roll()
{
	srand((unsigned)time(NULL));
	int rand_num = rand() % 6 + 1;
	setDiceVisible(rand_num);
	return rand_num;
}

void GameScene::planeMove(plane * p)
{
	MoveBy* moveBy;
	switch (p->direction)
	{
	case up:
		moveBy = MoveBy::create(0.15, Vec2(0, 1)*BlockSize);
		break;
	case down:
		moveBy = MoveBy::create(0.15, Vec2(0, -1)*BlockSize);
		break;
	case left:
		moveBy = MoveBy::create(0.15, Vec2(-1, 0)*BlockSize);
		break;
	case right:
		moveBy = MoveBy::create(0.15, Vec2(1, 0)*BlockSize);
		break;
	default:
		break;
	}
	p->spr->runAction(moveBy);
	SimpleAudioEngine::getInstance()->playEffect("music/fly.wav", false, 0.2f, 0.2f, 0.2f);
	p->position = p->spr->getPosition();
	dice_point--;
}

void GameScene::crush(plane * p)
{
	removeChild(p->spr);
}

void GameScene::setDiceVisible(int num)
{
	for (int i = 0; i < 7; i++) {
		if (i == num) {
			dice.at(i)->setVisible(true);
		}
		else {
			dice.at(i)->setVisible(false);
		}
	}
}

void GameScene::setDirection(plane * p, dir d)
{
	p->direction = d;
	switch (d)
	{
	case up:
		p->spr->setRotation(0);
		break;
	case down:
		p->spr->setRotation(180);
		break;
	case left:
		p->spr->setRotation(270);
		break;
	case right:
		p->spr->setRotation(90);
		break;
	default:
		break;
	}
}

int GameScene::getDistance(Vec2 pos1, Vec2 pos2)
{
	pos1 = pos1 / BlockSize;
	pos2 = pos2 / BlockSize;
	int distance = abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
	return distance;
}

bool GameScene::isChessBoardContain(Vec2 pos)
{
	for (auto block : chessBoard) {
		if (block->position == pos) {
			return true;
		}
	}
	return false;
}

int GameScene::getChoice(plane* p)
{
	Vec2 leftSide = Vec2(-1, 0)*BlockSize;
	Vec2 rightSide = Vec2(1, 0)*BlockSize;
	Vec2 upSide = Vec2(0, 1)*BlockSize;
	Vec2 downSide = Vec2(0, -1)*BlockSize;
	int choice = 0;
	switch (p->direction)
	{
	case up:
		if (isChessBoardContain(p->position + leftSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + upSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + rightSide)) {
			choice++;
		}
		break;
	case down:
		if (isChessBoardContain(p->position + leftSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + downSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + rightSide)) {
			choice++;
		}
		break;
	case left:
		if (isChessBoardContain(p->position + leftSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + downSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + upSide)) {
			choice++;
		}
		break;
	case right:
		if (isChessBoardContain(p->position + upSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + downSide)) {
			choice++;
		}
		if (isChessBoardContain(p->position + rightSide)) {
			choice++;
		}
		break;
	default:
		break;
	}
	return choice;
}

dir GameScene::getOnlyDirection(plane * p)
{
	switch (p->direction)
	{
	case up:
		if (isChessBoardContain(p->position + Vec2(-1, 0)*BlockSize)) {
			return left;
		}
		if (isChessBoardContain(p->position + Vec2(1, 0)*BlockSize)) {
			return right;
		}
		if (isChessBoardContain(p->position + Vec2(0, 1)*BlockSize)) {
			return up;
		}
		break;
	case down:
		if (isChessBoardContain(p->position + Vec2(-1, 0)*BlockSize)) {
			return left;
		}
		if (isChessBoardContain(p->position + Vec2(1, 0)*BlockSize)) {
			return right;
		}
		if (isChessBoardContain(p->position + Vec2(0, -1)*BlockSize)) {
			return down;
		}
		break;
	case left:
		if (isChessBoardContain(p->position + Vec2(-1, 0)*BlockSize)) {
			return left;
		}
		if (isChessBoardContain(p->position + Vec2(0, -1)*BlockSize)) {
			return down;
		}
		if (isChessBoardContain(p->position + Vec2(0, 1)*BlockSize)) {
			return up;
		}
		break;
	case right:
		if (isChessBoardContain(p->position + Vec2(1, 0)*BlockSize)) {
			return right;
		}
		if (isChessBoardContain(p->position + Vec2(0, -1)*BlockSize)) {
			return down;
		}
		if (isChessBoardContain(p->position + Vec2(0, 1)*BlockSize)) {
			return up;
		}
		break;
	default:
		break;
	}
}

void GameScene::Boom(plane * p) {
	ParticleExplosion* bao1 = ParticleExplosion::create();
	bao1->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
	bao1->setPosition(p->position);
	bao1->setScale(0.2f);
	addChild(bao1);
	SimpleAudioEngine::getInstance()->playEffect("music/boom.wav", false, 0.2f, 0.2f, 0.2f);
}

void GameScene::addBackground() {
	auto bgsprite = Sprite::create("background.jpg");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);
}

void GameScene::arrowUpdate()
{
	upArrow->setVisible(false);
	downArrow->setVisible(false);
	leftArrow->setVisible(false);
	rightArrow->setVisible(false);
	if (chosen_plane != nullptr && getChoice(chosen_plane) != 1) {
		switch (chosen_plane->direction)
		{
		case up:
			if (isChessBoardContain(chosen_plane->position + Vec2(-1, 0)*BlockSize)) {
				leftArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(1, 0)*BlockSize)) {
				rightArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, 1)*BlockSize)) {
				upArrow->setVisible(true);
			}
			break;
		case down:
			if (isChessBoardContain(chosen_plane->position + Vec2(-1, 0)*BlockSize)) {
				leftArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(1, 0)*BlockSize)) {
				rightArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, -1)*BlockSize)) {
				downArrow->setVisible(true);
			}
			break;
		case left:
			if (isChessBoardContain(chosen_plane->position + Vec2(-1, 0)*BlockSize)) {
				leftArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, -1)*BlockSize)) {
				downArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, 1)*BlockSize)) {
				upArrow->setVisible(true);
			}
			break;
		case right:
			if (isChessBoardContain(chosen_plane->position + Vec2(1, 0)*BlockSize)) {
				rightArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, -1)*BlockSize)) {
				downArrow->setVisible(true);
			}
			if (isChessBoardContain(chosen_plane->position + Vec2(0, 1)*BlockSize)) {
				upArrow->setVisible(true);
			}
			break;
		default:
			break;
		}
	}
}

bool GameScene::isGameOver()
{
	if (player1.empty() || player2.empty()) {
		return true;
	}
	return false;
}

void GameScene::chessBoardInit()
{
	for (int i = 0; i < 17; i++) {
		if (i < 4) {
			//chessBoard.pushBack(Block::create((color)red, Vec2(5, i + 1)));
		}
		else {
			if (i % 4 == 0) {
				chessBoard.pushBack(Block::create((color)red, Vec2(5, i + 1)));
			}
			if (i % 4 == 1) {
				chessBoard.pushBack(Block::create((color)blue, Vec2(5, i + 1)));
			}
			if (i % 4 == 2) {
				chessBoard.pushBack(Block::create((color)yellow, Vec2(5, i + 1)));
			}
			if (i % 4 == 3) {
				chessBoard.pushBack(Block::create((color)green, Vec2(5, i + 1)));
			}
		}
	}
	for (int i = 0; i < 17; i++) {
		if (i < 4) {
			//chessBoard.pushBack(Block::create((color)blue, Vec2(i + 1, 18)));
		}
		else {
			if (i % 4 == 0) {
				chessBoard.pushBack(Block::create((color)blue, Vec2(i + 1, 18)));
			}
			if (i % 4 == 1) {
				chessBoard.pushBack(Block::create((color)yellow, Vec2(i + 1, 18)));
			}
			if (i % 4 == 2) {
				chessBoard.pushBack(Block::create((color)green, Vec2(i + 1, 18)));
			}
			if (i % 4 == 3) {
				chessBoard.pushBack(Block::create((color)red, Vec2(i + 1, 18)));
			}
		}
	}
	for (int i = 0; i < 17; i++) {
		if (i < 4) {
			//chessBoard.pushBack(Block::create((color)yellow, Vec2(18, 22 - i)));
		}
		else {
			if (i % 4 == 0) {
				chessBoard.pushBack(Block::create((color)yellow, Vec2(18, 22 - i)));
			}
			if (i % 4 == 1) {
				chessBoard.pushBack(Block::create((color)green, Vec2(18, 22 - i)));
			}
			if (i % 4 == 2) {
				chessBoard.pushBack(Block::create((color)red, Vec2(18, 22 - i)));
			}
			if (i % 4 == 3) {
				chessBoard.pushBack(Block::create((color)blue, Vec2(18, 22 - i)));
			}
		}
	}
	for (int i = 0; i < 17; i++) {
		if (i < 4) {
			//chessBoard.pushBack(Block::create((color)green, Vec2(22 - i, 5)));
		}
		else {
			if (i % 4 == 0) {
				chessBoard.pushBack(Block::create((color)green, Vec2(22 - i, 5)));
			}
			if (i % 4 == 1) {
				chessBoard.pushBack(Block::create((color)red, Vec2(22 - i, 5)));
			}
			if (i % 4 == 2) {
				chessBoard.pushBack(Block::create((color)blue, Vec2(22 - i, 5)));
			}
			if (i % 4 == 3) {
				chessBoard.pushBack(Block::create((color)yellow, Vec2(22 - i, 5)));
			}
		}
	}
	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			chessBoard.pushBack(Block::create((color)white, Vec2(6 + i, 11)));
		}
		else {
			chessBoard.pushBack(Block::create((color)black, Vec2(6 + i, 11)));
		}
	}
	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			chessBoard.pushBack(Block::create((color)white, Vec2(11, 17 - i)));
		}
		else {
			chessBoard.pushBack(Block::create((color)black, Vec2(11, 17 - i)));
		}
	}
	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			chessBoard.pushBack(Block::create((color)white, Vec2(17 - i, 12)));
		}
		else {
			chessBoard.pushBack(Block::create((color)black, Vec2(17 - i, 12)));
		}
	}
	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			chessBoard.pushBack(Block::create((color)white, Vec2(12, 6 + i)));
		}
		else {
			chessBoard.pushBack(Block::create((color)black, Vec2(12, 6 + i)));
		}
	}
	for (auto block : chessBoard) {
		block->position *= BlockSize;
		block->sprite->setPosition(block->position);
		addChild(block->sprite, 0);
	}
	for (int i = 0; i < 7; i++) {
		Sprite* temp;

		if (i == 0) {
			temp = Sprite::create("dice.png");
			temp->setVisible(true);
			temp->setPosition(Vec2(7, 3) * BlockSize);
			dice.pushBack(temp);
		}
		else {
			char filename[20];
			sprintf(filename, "d%d.png", i);
			temp = Sprite::create(filename);
			temp->setVisible(false);
			temp->setPosition(Vec2(7, 3) * BlockSize);
			dice.pushBack(temp);
		}
		addChild(temp, 0);
	}

	player1_skill_indicator = Sprite::create("rPlane.png");
	player1_skill_indicator->setPosition(Vec2(2, 11)*BlockSize);
	player1_skill_indicator->setVisible(false);
	addChild(player1_skill_indicator, 0);

	player2_skill_indicator = Sprite::create("bPlane.png");
	player2_skill_indicator->setPosition(Vec2(2, 9)*BlockSize);
	addChild(player2_skill_indicator, 0);

	indicator_red = Sprite::create("rPlane.png");
	indicator_blue = Sprite::create("bPlane.png");
	indicator_red->setPosition(Vec2(11, 21)*BlockSize);
	indicator_blue->setPosition(Vec2(11, 21)*BlockSize);
	addChild(indicator_red, 0);
	addChild(indicator_blue, 0);

	indicator = Label::createWithTTF("ROLL", "fonts/arial.ttf", 20);
	indicator->setPosition(Vec2(11, 20)*BlockSize);
	addChild(indicator, 0);

	dice_point_indicator = Label::createWithTTF("0", "fonts/arial.ttf", 25);
	dice_point_indicator->setPosition(Vec2(13, 21)*BlockSize);
	addChild(dice_point_indicator, 0);

	upArrow->setRotation(270);   upArrow->setPosition(Vec2(5, 21.5)*BlockSize);  upArrow->setVisible(false); addChild(upArrow);
	downArrow->setRotation(90);   downArrow->setPosition(Vec2(5, 19.5)*BlockSize); downArrow->setVisible(false);  addChild(downArrow);
	leftArrow->setRotation(180);   leftArrow->setPosition(Vec2(3.5, 20.5)*BlockSize);  leftArrow->setVisible(false); addChild(leftArrow);
	rightArrow->setPosition(Vec2(6.5, 20.5)*BlockSize); rightArrow->setVisible(false);  addChild(rightArrow);

}
void GameScene::playerInit_1()
{
	player1.pushBack(plane::create(red, Vec2(2, 4) * BlockSize, up));
	player1.pushBack(plane::create(red, Vec2(4, 4) * BlockSize, up));
	player1.pushBack(plane::create(red, Vec2(2, 2) * BlockSize, up));
	player1.pushBack(plane::create(red, Vec2(4, 2) * BlockSize, up));
	for (int i = 0; i < 4; i++) {
		player1.at(i)->spr->setPosition(player1.at(i)->position);
		addChild(player1.at(i)->spr, 1);
	}
	
}
void GameScene::playerInit_2()
{
	player2.pushBack(plane::create(blue, Vec2(19, 21) * BlockSize, down));
	player2.pushBack(plane::create(blue, Vec2(21, 21) * BlockSize, down));
	player2.pushBack(plane::create(blue, Vec2(19, 19) * BlockSize, down));
	player2.pushBack(plane::create(blue, Vec2(21, 19) * BlockSize, down));
	for (int i = 0; i < 4; i++) {
		player2.at(i)->spr->setPosition(player2.at(i)->position);
		addChild(player2.at(i)->spr, 1);
	}
}
void GameScene::buttonInit()
{
	MenuItemFont::setFontName("fonts/arial.ttf");
	MenuItemFont::setFontSize(35);
	auto button_1 = MenuItemFont::create("1", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(1);
			dice_point = 1;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_2 = MenuItemFont::create("2", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(2);
			dice_point = 2;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_3 = MenuItemFont::create("3", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(3);
			dice_point = 3;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_4 = MenuItemFont::create("4", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(4);
			dice_point = 4;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_5 = MenuItemFont::create("5", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(5);
			dice_point = 5;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_6 = MenuItemFont::create("6", [&](Ref* pSender) {
		if (state == _await) {
			setDiceVisible(6);
			dice_point = 6;
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	auto button_Roll = MenuItemFont::create("ROLL", [&](Ref* pSender) {
		if (state == _await) {
			dice_point = roll();
			if (turn == red) {
				if (player1_kill == true) {
					schedule(schedule_selector(GameScene::updateCustom), 0.40);
					player1_kill = false;
				}
				else {
					state = plane_choose;
				}
			}
			else if (turn == blue) {
				bool temp = true;
				for (int i = 0; i < player2.size(); i++) {
					if (isChessBoardContain(player2.at(i)->position)) {
						temp = false;
						break;
					}
				}
				if (temp == true && dice_point != 6) {
					dice_point = 0;
					turn = red;
				}
				else {
					state = plane_choose;
				}
			}
		}
	});
	button_1->setPosition(Vec2(9, 3) * BlockSize);
	button_2->setPosition(Vec2(11, 3) * BlockSize);
	button_3->setPosition(Vec2(13, 3) * BlockSize);
	button_4->setPosition(Vec2(15, 3) * BlockSize);
	button_5->setPosition(Vec2(17, 3) * BlockSize);
	button_6->setPosition(Vec2(19, 3) * BlockSize);
	button_Roll->setPosition(Vec2(7, 1.5) * BlockSize);

	auto menu = Menu::create(button_1, button_2, button_3, button_4, button_5, button_6, button_Roll, nullptr);
	menu->setPosition(Vec2::ZERO);
	addChild(menu);
}
