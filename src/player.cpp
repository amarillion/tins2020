#include <assert.h>
#include "player.h"
#include "resources.h"
#include "anim.h"
#include "engine.h"
#include "door.h"
#include "game.h"
#include "mainloop.h"
#include "util.h"

Anim *Bullet::bullet[1];

Bullet::Bullet(Room *r, int _dir, int _range, int _damage, Player *_parent) : Object (r, _parent ? OT_BULLET : OT_ENEMY_BULLET)
{
	const int speed_factor = 6;
	solid = false;
	setAnim (bullet[0], type == OT_BULLET ? 0 : 1);
	dx = (al_fixed)(dir_mult[0][_dir] * speed_factor);
	dy = (al_fixed)(dir_mult[1][_dir] * speed_factor);
	setDir (_dir);
	range = _range;
	damage = _damage;
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
	if (type == OT_BULLET && o->getType() == OT_MONSTER) // bullet hits monster
	{
		kill();
	}
	if (type == OT_ENEMY_BULLET && o->getType() == OT_PLAYER) // enemy bullet hits player
	{
		kill();
	}
}
	
void PickUp::handleCollission(ObjectBase *o)
{
	if (o->getType() == OT_PLAYER)
	{
		kill();
	}
}

void PickUp::init(Resources *res)
{
	anims[AnimType::BANANA] = res->getAnim("banana");
	anims[AnimType::HEALTH] = res->getAnim("health");
	anims[AnimType::KEY] = res->getAnim("key");
	anims[AnimType::BONUS1] = res->getAnim("bonus1");
	anims[AnimType::BONUS2] = res->getAnim("bonus2");
	anims[AnimType::BONUS3] = res->getAnim("bonus3");
	anims[AnimType::BONUS4] = res->getAnim("bonus4");
}

Anim *PickUp::anims[PickUp::AnimType::ANIM_NUM];

Player::Player(PlayerState *_ps, Room *r, int _playerType) : Object (r, OT_PLAYER)
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
ALLEGRO_SAMPLE *Player::samples[Player::SAMPLE_NUM];
ALLEGRO_SAMPLE *Player::shoot[6];

void Player::init(Resources *res)
{
	walk[0] = res->getAnim ("fole_walk");
	walk[1] = res->getAnim ("raul_walk");

 	samples[HURT1] = res->getSample ("miau2");
 	samples[HURT2] = res->getSample ("uh");
 	samples[PICKUP_KEY] = res->getSample ("Pick_up_key_2");
 	samples[PICKUP_OTHER] = res->getSample ("Pick_up_key_1");
 	samples[UNLOCK] = res->getSample ("Door_unlock");
 	samples[STEPS] = res->getSample ("Footsteps_double");
 	samples[TELEPORT] = res->getSample ("AlienSiren");

	shoot[0] = res->getSample ("Strum_1");
	shoot[1] = res->getSample ("Strum_2");
	shoot[2] = res->getSample ("Strum_3");
	shoot[3] = res->getSample ("Strum_4");
	shoot[4] = res->getSample ("Strum_5");
	shoot[5] = res->getSample ("Strum_6");

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
			int idx = random(6);
			MainLoop::getMainLoop()->playSample (shoot[idx]);
	
			// generate bullet...
			Bullet *bullet = new Bullet(getRoom(), getDir(), ps->wpnRange, ps->wpnDamage, this);
			game->getObjects()->add (bullet);
			bullet->setLocation (getx() + 8, gety());

			if (ps->wpnType > 0) {
				{
				Bullet *bullet = new Bullet(getRoom(), (getDir() + 2) % 4, ps->wpnRange, ps->wpnDamage, this);
				game->getObjects()->add (bullet);
				bullet->setLocation (getx() + 8, gety());
				}
				{
				Bullet *bullet = new Bullet(getRoom(), (getDir() + 3) % 4, ps->wpnRange, ps->wpnDamage, this);
				game->getObjects()->add (bullet);
				bullet->setLocation (getx() + 8, gety());
				}
			}
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
	
	MainLoop::getMainLoop()->playSample (samples[playerType == 0 ? HURT1 : HURT2]);
	
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
	if (o->getType() == OT_BANANA) // pick-up
	{
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		ps->bananas++;
	}
	else if (o->getType() == OT_KEY) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_KEY]);
		ps->keys++;
	}
	else if (o->getType() == OT_HEALTH) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		ps->hp = ps->hpMax;
		game->messages->showMessage("Full Health", Messages::RIGHT_TO_LEFT);
	}
	else if (o->getType() == OT_BONUS1) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		game->addTime(30000); // add 30 seconds
	}
	else if (o->getType() == OT_BONUS2) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		ps->wpnRange += 40;
		game->messages->showMessage("Increased weapon range", Messages::RIGHT_TO_LEFT);
	}
	else if (o->getType() == OT_BONUS3) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		ps->wpnDamage += 2;
		game->messages->showMessage("Increased weapon power", Messages::RIGHT_TO_LEFT);
	}
	else if (o->getType() == OT_BONUS4) {
		MainLoop::getMainLoop()->playSample (samples[PICKUP_OTHER]);
		ps->wpnType = 1;
		game->messages->showMessage("Weapon upgrade", Messages::RIGHT_TO_LEFT);
	}
	else if (o->getType() == OT_MONSTER) // monster
	{
		if (hittimer == 0) hit(10); // TODO: damage determined by monster
	}
	else if (o->getType() == OT_LOCKED_DOOR) {
		if (ps->keys > 0) {
			Door *d = dynamic_cast<Door*>(o);			
			d->setLocked(false);
			MainLoop::getMainLoop()->playSample (samples[UNLOCK]);
			ps->keys--;
			// transportCounter = transportDelay; // avoid going through straight away
		}
	}	
	else if (o->getType() == OT_DOOR) // exit
	{		
		if (transportCounter == 0)
		{
			Door *d = dynamic_cast<Door*>(o);
			assert (d);
			MainLoop::getMainLoop()->playSample (samples[STEPS]);
			if (d->otherRoom != NULL)
			{
				setRoom(d->otherRoom);
				setLocation(d->otherDoor->getx(), d->otherDoor->gety());
			}
		}
		transportCounter = transportDelay; // make sure we don't go back
	}
	else if (o->getType() == OT_TELEPORT)
	{
		if (transportCounter == 0)
		{
			Door *t = dynamic_cast<Door*>(o);
			assert (t);
			MainLoop::getMainLoop()->playSample (samples[TELEPORT]);
			if (t->otherRoom != NULL)
			{
				setRoom(t->otherRoom);
				setLocation(t->otherDoor->getx(), t->otherDoor->gety());
			}
		}
		transportCounter = transportDelay; // make sure we don't go back		
	}
	else if (o->getType() == OT_ENEMY_BULLET)
	{		
		if (hittimer == 0) hit(10); // TODO: damage determined by bullet
	}
}
