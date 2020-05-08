#include <assert.h>
#include "player.h"
#include "resources.h"
#include "anim.h"
#include "engine.h"
#include "door.h"
#include "game.h"
#include "mainloop.h"

Anim *Bullet::bullet[1];

Bullet::Bullet(Room *r, int _dir, int _range, int _damage, BulletType _type, Player *_parent) : Object (r)
{
	const int speed_factor = 6;
	solid = false;
	setAnim (bullet[0]);
	dx = (al_fixed)(dir_mult[0][_dir] * speed_factor);
	dy = (al_fixed)(dir_mult[1][_dir] * speed_factor);
	setDir (_dir);
	range = _range;
	damage = _damage;
	type = _type;
	parent = _parent;
}

void Bullet::init(Resources *res)
{
	bullet[0] = res->getAnim("bullet");
}

void Bullet::update()
{
	try_move (dx, dy);
	range -= ((dx + dy) < 0 ? -dx - dy : dx + dy); // we take a shortcut by assuming that dx or dy is 0

	if (range <= 0) 
	{
		kill();
		return;
	}
		
	int mx1, my1, mx2, my2;
	int ix, iy;
	TEG_MAP *map = getMap();
	mx1 = ((int)getx()) / map->tilelist->tilew;
	my1 = ((int)gety()) / map->tilelist->tileh;
	mx2 = ((int)getx() + w - 1) / map->tilelist->tilew;
	my2 = ((int)gety() + h - 1) / map->tilelist->tileh;
			
	// loop through all map positions we touch with the solid region
	for (ix = mx1; ix <= mx2; ++ix)
	{
		for (iy = my1; iy <= my2; ++iy)
		{
			// see if there is a solid tile at this position
			if (getTileStackFlags (ix, iy) & TS_SOLID)
			{
				// leave a splash
				kill();
			}
		}
	}
}


void Bullet::handleCollission(ObjectBase *o)
{
	if (o->getType() == OT_MONSTER) // monster
	{
		kill();
	}
}
	
void Banana::handleCollission(ObjectBase *o)
{
	if (o->getType() == OT_PLAYER)
	{
		kill();
	}
}

void Banana::init(Resources *res)
{
	banana = res->getAnim("banana");
}

Anim *Banana::banana;


Player::Player(PlayerState *_ps, Room *r, int _playerType) : Object (r)
{
	ps = _ps;
	transportCounter = 0;
	setVisible(true);
	hittimer = 0;
	attacktimer = 0;
	button = NULL;
	playerType = _playerType;
	solid = true;
	
	setAnim (walk[playerType]);	
}

Anim *Player::walk[2];
ALLEGRO_SAMPLE *Player::hurt[2];

void Player::init(Resources *res)
{
	walk[0] = res->getAnim ("fole_walk");
	walk[1] = res->getAnim ("raul_walk");

 	hurt[0] = res->getSample ("miau2");
 	hurt[1] = res->getSample ("uh");
}

void Player::update()
{
	if (!button) button = engine->getInput(playerType);
	assert (button);

	// reduce transportCounter
	// when touching a transport (door or teleport), transportCounter is increased again in handlecollision.
	if (transportCounter > 0)
	{
		transportCounter--;
	}
	
	if (hittimer > 0)
	{
		hittimer--;
		setVisible (!(hittimer & 1));
	}
	
	bool freshAnim = false;
		
	// attacking
	if (attacktimer > 0)
	{
		attacktimer --;
	}
	else
	{
		if (button[btnAction].getState())
		{
			attacktimer = ps->wpnSpeed;
// 			parent->getParent()->playSample (shoot);

			// generate bullet...
			Bullet *bullet = new Bullet(getRoom(), getDir(), ps->wpnRange, ps->wpnDamage, Bullet::BT_NORMAL, this);
			game->getObjects()->add (bullet);
			bullet->setLocation (getx() + 8, gety());
		}
	}
	
	al_fixed dx = al_fixed(0), dy = al_fixed(0);

	const al_fixed speedFactor = al_fixed(3);
	// walking
	if (button[btnLeft].getState())
	{
		dx = -speedFactor;
		if (getDir() != LEFT) freshAnim = true;
		setDir(LEFT);
		isWalking = true;
	} else if (button[btnRight].getState())
	{
		dx = speedFactor;
		if (getDir() != RIGHT) freshAnim = true;
		setDir(RIGHT);
		isWalking = true;
	} else if (button[btnUp].getState())
	{
		dy = -speedFactor;
		if (getDir() != UP) freshAnim = true;
		setDir(UP);
		isWalking = true;
	} else if (button[btnDown].getState())
	{
		dy = speedFactor;
		if (getDir() != DOWN) freshAnim = true;
		setDir(DOWN);
		isWalking = true;
	} else
	{
		isWalking = false;
	}
	
	if (freshAnim)
	{
		setAnim (walk[playerType]);
	}
	
	try_move (dx, dy);
	
	// handle animation
	Object::update();
}

void Player::hit(int damage)
{
	hittimer = 50;
	
	MainLoop::getMainLoop()->playSample (hurt[playerType]);
	
	ps->hp -= damage;
	if (ps->hp <= 0)
	{
		ps->died = true;
		setVisible (false);
		solid = false;
	}	
}

void Player::handleCollission (ObjectBase *o)
{
	if (ps->died) return;
	if (o->getType() == OT_BANANA) // monster
	{
		ps->bananas++;
	}	
	if (o->getType() == OT_MONSTER) // monster
	{
		if (hittimer == 0) hit(10); // TODO: damage determined by monster
	}	
	if (o->getType() == OT_DOOR) // exit
	{		
		if (transportCounter == 0)
		{
			Door *d = dynamic_cast<Door*>(o);
			assert (d);
			if (d->otherRoom != NULL)
			{
				setRoom(d->otherRoom);
				setLocation(d->otherDoor->getx(), d->otherDoor->gety());
			}
		}
		transportCounter = transportDelay; // make sure we don't go back
	}
	if (o->getType() == OT_TELEPORT)
	{
		if (transportCounter == 0)
		{
			Teleport *t = dynamic_cast<Teleport*>(o);
			assert (t);
			if (t->otherRoom != NULL)
			{
				setRoom(t->otherRoom);
				setLocation(t->otherTeleport->getx(), t->otherTeleport->gety());
			}
		}
		transportCounter = transportDelay; // make sure we don't go back		
	}
	if (o->getType() == OT_BULLET) // enemy bullet (only in bananas mode)
	{		
		
	}
}

PlayerState::PlayerState()
{
	// player stats	
	hp = defaultHpMax; // hitpoints
	hpMax = defaultHpMax; // max hitpoints
	wpnSpeed = defaultWpnSpeed; // weapon rate of fire
	wpnRange = defaultWpnRange; // weapon range, distance bullets can go
	wpnDamage = defaultWpnDamage; 
	bananas = 0; // bananas collected
	xp = 0; // gold collected for killing monsters
	died = false;
}
