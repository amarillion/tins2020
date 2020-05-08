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
		virtual int getType () { return OT_BULLET; }
		virtual void handleCollission(ObjectBase *o);
		static void init(Resources *res);
		
		static Anim *bullet[1];
};

class Banana : public Object
{
	public:		
		Banana(Room *r) : Object (r) { solid = true; setAnim (banana); }
		virtual int getType () { return OT_BANANA; }
		virtual void handleCollission(ObjectBase *o);
		static void init(Resources *res);		
		static Anim *banana;
};

class PlayerState
{
	public:
	static const int defaultWpnSpeed = 20;
	static const int defaultWpnRange = 120;
	static const int defaultWpnDamage = 4;
	static const int defaultHpMax= 100;
	
	// player stats	
	int hp; // hitpoints
	int hpMax; // max hitpoints
	int wpnSpeed; // weapon rate of fire
	int wpnRange; // weapon range, distance bullets can go
	int wpnDamage; 
	int bananas; // bananas collected
	int xp; // gold collected for killing monsters
	bool died;
	
	PlayerState();
};

class Player : public Object
{
	static const int transportDelay = 5;
	
	int transportCounter;

	static Anim *walk[2];
	static ALLEGRO_SAMPLE *hurt[2];
	
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
	virtual int getType () { return OT_PLAYER; }
	virtual void handleCollission(ObjectBase *o);
};

#endif
