#ifndef MONSTER_H
#define MONSTER_H

#include "object.h"
// #include <lua.h> //TODO

const int MONSTER_NUM = 4;

class Monster : public Object
{
private:
	static const int defaultHitCount = 20;
	static const int defaultHitPoints = 20;
	static const int defaultXpValue = 24;
	static const int chaseRadius = 160;
	
	static Anim *sprites[MONSTER_NUM];
	static Anim *hitsprites[MONSTER_NUM];
	static ALLEGRO_SAMPLE *samples[MONSTER_NUM];
	int monsterType;
	int hp;
	int count;
	int hitCount;
//	lua_State* state; //TODO
public:
	Monster(Room *r, int type, int _hp);
	static void init(Resources *res);
	virtual void update() override;

	virtual void handleCollission(ObjectBase *o);
};

#endif
