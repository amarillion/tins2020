#include <assert.h>
#include "monster.h"
#include "resources.h"
#include "engine.h"
#include <stdlib.h>
// #include <lua.h> //TODO
#include "game.h"
#include "mainloop.h"

Anim *Monster::sprites[MONSTER_NUM];
ALLEGRO_SAMPLE *Monster::samples[MONSTER_NUM];

static void *l_alloc (void *ud, void *ptr, size_t osize,
                                           size_t nsize) {
  (void)ud;  (void)osize;  /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
}

Monster::Monster(Room *r, int type, int _hp) : Object (r), monsterType (type)
{
	setVisible(true);
	solid = true;
	setAnim(sprites[monsterType]);
	count = 100;
	setDir (rand() % 4);
	hitCount = 0;
	hp = _hp;

//	state = lua_newstate(l_alloc, NULL);
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

void Monster::update()
{
	count--;
	if (count == 0)
	{
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
		count = rand() % 50 + 25;
	}
	
	const int speedFactor = 2;
	
	al_fixed dx = (al_fixed)(speedFactor * dir_mult[0][getDir()]);
	al_fixed dy = (al_fixed)(speedFactor * dir_mult[1][getDir()]);
	try_move (dx, dy);
	
	if (hitCount > 0) 
	{ 
		hitCount--; 
		if (hitCount == 0) setAnim(sprites[monsterType]);
	}
	
	if (hp < 0 && hitCount <= 0) { kill(); }

	//lua io.write("Hello world, from ",_VERSION,"!\n")

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
		setAnim (sprites[monsterType], 1);
		hitCount = defaultHitCount;
		MainLoop::getMainLoop()->playSample (samples[monsterType]);
	}	
}


