#include <assert.h>
#include "monster.h"
#include "resources.h"
#include "engine.h"
#include <stdlib.h>
// #include <lua.h> //TODO
#include "game.h"
#include "mainloop.h"
#include "util.h"

Anim *Monster::sprites[MONSTER_NUM];
ALLEGRO_SAMPLE *Monster::samples[MONSTER_NUM];

Monster::Monster(Room *r, int subType, int _hp) : Object (r, OT_MONSTER), monsterType (subType)
{
	setVisible(true);
	solid = true;
	setAnim(sprites[monsterType]);
	setDir (rand() % 4);
	hp = _hp;
}

void Monster::init(Resources *res)
{
	sprites[0] = res->getAnim ("monster1");
	sprites[1] = res->getAnim ("monster2");
	sprites[2] = res->getAnim ("monster3");
	sprites[3] = res->getAnim ("monster4");
	
	samples[0] = res->getSample ("fugly2");
	samples[1] = res->getSample ("fugly2");
	samples[2] = res->getSample ("mugly2");
	samples[3] = res->getSample ("tux");
}

void Monster::newState(State newState, int time) {
	switch(newState) {
		case MOVERANDOM: {
			setState(0);
			Player *p = game->getNearestPlayer (this);
			if (p && (rand() % 100 > 30))
			{
				int dx = p->getx() - getx();
				int dy = p->gety() - gety();
				if (dx * dx + dy * dy < chaseRadius * chaseRadius)
				{
					if (abs (dx) > abs(dy))
					{
						if (dx > 0)				
							setDir (RIGHT);
						else
							setDir (LEFT);
					}
					else
					{
						if (dy > 0)				
							setDir (DOWN);
						else
							setDir (UP);				
					}
				}
				else
				{
					setDir (rand() % 4);
				}				
			}
			else
			{
				setDir (rand() % 4);
			}
		}
			break;
		case WAIT:
			setState (3);
			break;
		case KNOCKBACK:
			setState (hp < 0 ? 2 : 1);
			break;
		case SHOOT:
			setState (4);
			break;
	}
	count = time;
	eState = newState;
};

void Monster::determineNextState() {
	switch (monsterType) {
		case 0: // Flower
			if (eState != SHOOT && random(100) > 30) {
				newState(SHOOT, defaultTelegraphingDelay);
			}
			else {
				if (eState == WAIT) {
					newState(MOVERANDOM, rand() % 50 + 25);
				}
				else {
					newState(WAIT, rand() % 50 + 25);
				}
			}
			break;
		case 1: // Bell
			newState(SHOOT, rand() % 80 + 50);
			break;
		case 2: // One bug
			if (eState == WAIT) {
				newState(MOVERANDOM, rand() % 50 + 25);
			}
			else {
				newState(WAIT, rand() % 50 + 25);
			}
			break;
		default: // Other bugs
			newState(MOVERANDOM, rand() % 50 + 25);
			break;
	}
}

void Monster::update()
{
	// update on a given state
	count--;
	if (count <= 0)
	{
		// finish action determine next action..
		switch(eState) {
			case KNOCKBACK:
				if (hp < 0) { kill(); }
				break;
			case SHOOT:
				for (int i = 0; i < 4; ++i) {
					Bullet *bullet = new Bullet(getRoom(), i, defaultWeaponRange, 
						defaultWeaponDmg, nullptr);
					game->getObjects()->add (bullet);
					bullet->setLocation (getx() + 8, gety());
				}
				break;
			default:
				// do nothing
				break;
		}

		determineNextState();
	}

	// continue doing current action...
	int speedFactor = 0;

	switch (eState) {
		case KNOCKBACK:
			speedFactor = 4;
			break;
		case WAIT:
			speedFactor = 0;
			break;
		case SHOOT:
			speedFactor = 0;
			break;
		case MOVERANDOM:
			speedFactor = 2;
			break;
	};

	if (speedFactor > 0) {
		al_fixed dx = (al_fixed)(speedFactor * dir_mult[0][getDir()]);
		al_fixed dy = (al_fixed)(speedFactor * dir_mult[1][getDir()]);
		try_move (dx, dy);
	}
		
	Object::update();
}

void Monster::handleCollission(ObjectBase *o)
{
	if (o->getType() == OT_BULLET) // player attack
	{
		Bullet *b = dynamic_cast<Bullet *>(o);
		Player *p = b->getPlayer();
		hp -= p->ps->wpnDamage;
		if (hp < 0)
		{
			p->ps->xp += defaultXpValue;
		} // killed after hitcount == 0 again
		
		// bullet impact changes dir
		setDir (b->getDir());
		
		newState(KNOCKBACK, defaultHitDelay);
		MainLoop::getMainLoop()->playSample (samples[monsterType]);
	}	
}


