#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "settings.h"

class Resources;
class Player;
	
class Bullet : public Object
{
	public:
		enum BulletType { BT_NORMAL = 0 };
	
	private:
		
		al_fixed dx, dy;
		al_fixed range; // range in pixels
		
		int damage;
		BulletType type;
		Player *parent;
	public:
		Player *getPlayer() { return parent; }
		Bullet(Room *r, int _dir, int _range, int _damage, BulletType _type, Player *_parent);
		
		virtual void update() override;
		virtual void handleCollission(ObjectBase *o);
		static void init(Resources *res);
		
		static Anim *bullet[1];
};

class PickUp : public Object
{
	public:		
		PickUp(Room *r, int type) : Object (r, type) { 
			solid = true; 
			switch(type) {
				case OT_BANANA: setAnim(banana); break;
				case OT_HEALTH: setAnim(health); break;
				case OT_KEY: setAnim(key); break;
			}

		}
		virtual void handleCollission(ObjectBase *o);
		static void init(Resources *res);		
		
		static Anim *banana, *key, *health;
};

class PlayerState
{
	public:
	static const int defaultWpnSpeed = 20;
	static const int defaultWpnRange = 120;
	static const int defaultWpnDamage = 4;
	static const int defaultHpMax= 100;
	
	// player stats	
	int hp = defaultHpMax; // hitpoints
	int hpMax = defaultHpMax; // max hitpoints
	int wpnSpeed = defaultWpnSpeed; // weapon rate of fire
	int wpnRange = defaultWpnRange; // weapon range, distance bullets can go
	int wpnDamage = defaultWpnDamage; 
	int bananas = 0; // bananas collected
	int keys = 0; // keys collected
	int xp = 0; // gold collected for killing monsters
	bool died = false;
};

class Player : public Object
{
	static const int transportDelay = 5;
	
	int transportCounter;

	static Anim *walk[2];
	static ALLEGRO_SAMPLE *shoot[6];
	enum Samples { HURT1, HURT2, UNLOCK, STEPS, PICKUP_KEY, PICKUP_OTHER, SAMPLE_NUM };
	static ALLEGRO_SAMPLE *samples[SAMPLE_NUM];
	
	int hittimer;
	int attacktimer;
	bool isWalking;
		
	int playerType; // 0 for fole, 1 for raul
	
	Input *button;
	
	void hit(int damage);
public:
	PlayerState *ps;
	Player(PlayerState *ps, Room *r, int _playerType);
	virtual void update() override;
	static void init(Resources *res);
	virtual void handleCollission(ObjectBase *o);
};

#endif
