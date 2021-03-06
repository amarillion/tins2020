#ifndef MONSTER_H
#define MONSTER_H

#include "object.h"
// #include <lua.h> //TODO

const int MONSTER_NUM = 4;

class Monster : public Object
{
private:
	static const int defaultHitDelay = 20;
	static const int defaultTelegraphingDelay = 30;
	static const int defaultHitPoints = 20;
	static const int defaultXpValue = 24;
	static const int defaultWeaponRange = 160;
	static const int defaultWeaponDmg = 2;
	static const int chaseRadius = 160;
	
	static Anim *sprites[MONSTER_NUM];
	static Anim *hitsprites[MONSTER_NUM];
	static ALLEGRO_SAMPLE *samples[MONSTER_NUM];
	int monsterType;
	int hp;
	int count = 0;

	enum State { WAIT, MOVERANDOM, SHOOT, KNOCKBACK };
	State eState = WAIT;
public:
	Monster(Room *r, int type, int _hp);
	static void init(Resources *res);

	virtual void update() override;
	virtual void handleCollission(ObjectBase *o);

	virtual void newState(State newState, int time);
	virtual void determineNextState();
};

#endif
