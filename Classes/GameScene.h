#ifndef __GAME_SEBCE_H__
#define __GAME_SEBCE_H__

#include "cocos2d.h"
#include "Plane.h"
#include "Block.h"
#include "CCShake.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

enum Game_State
{
	_await, plane_choose, dir_choose, over
};

class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void update(float f) override;
	void updateCustom(float dt);
	void updateOnce(float dt);


	void chessBoardInit();
	void playerInit_1();
	void playerInit_2();
	void buttonInit();

	CREATE_FUNC(GameScene);

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	int roll();
	void planeMove(plane*);
	void GameScene::Player0Move(int t);
	void GameScene::Player1Move(int t);
	void GameScene::Player2Move(int t);
	void GameScene::Player3Move(int t);
	void crush(plane* p);
	void setDiceVisible(int i);
	void setDirection(plane*, dir);
	int getDistance(Vec2, Vec2);
	bool isChessBoardContain(Vec2);
	int getChoice(plane*);
	dir getOnlyDirection(plane*);
	void Boom(plane * p);
	void addBackground();
	void arrowUpdate();
	bool isGameOver();
private:
	Sprite* indicator_red;
	Sprite* indicator_blue;
	Label* indicator;
	Label* dice_point_indicator;

	Sprite* player1_skill_indicator;
	Sprite* player2_skill_indicator;
	bool player1_skill_activated = false;
	bool player1_kill = false;
	bool player2_skill_activated = false;

	Sprite* upArrow = Sprite::create("arrow.png");
	Sprite* downArrow = Sprite::create("arrow.png");
	Sprite* leftArrow = Sprite::create("arrow.png");
	Sprite* rightArrow = Sprite::create("arrow.png");

	color turn = red;
	Game_State state = _await;
	int dice_point = 0;
	plane* chosen_plane;
	int BlockSize = 31;
	Vector<plane*> player1;
	Vector<plane*> player2;
	Vector<Block*> chessBoard;
	Vector<Sprite*> dice;
};

#endif // __GAME_SEBCE_H__
