#ifndef OBJECT_H
#define OBJECT_H

#include <allegro5/allegro.h>
#include <list>
#include <map>
#include "tegel5.h"
#include "level.h"
#include "component.h"
#include "objectbase.h"

// tile stack properties
#define TS_SOLID 0x01

class Objects;
class Engine;
class Anim;
class Resources;

enum {UP, DOWN, LEFT, RIGHT};

enum { 
	OT_MONSTER, OT_BULLET, OT_PLAYER, OT_DOOR, OT_LOCKED_DOOR, OT_TELEPORT, 
	
	OT_BANANA,

	OT_HEALTH,
	OT_KEY,
	OT_BONUS1,
	OT_BONUS2,
	OT_BONUS3,
	OT_BONUS4

};

static const int dir_mult[2][4] = { { 0, 0, -1, 1 }, {-1, 1, 0, 0 } };

class Room;
class Game;

class Object : public ObjectBase
{
protected:
	static Engine *engine;
	static Game *game;

	Room *room;
	
	int type;
	int getTileStackFlags(int mx, int my);
public:
	Object (Room *r, int type);
	virtual ~Object () {}
	
	TEG_MAP *getMap();
	void try_move (al_fixed dx, al_fixed dy);
	void setRoom (Room *_room);
	Room *getRoom () { return room; }

	virtual int getType() { return type; }
	static void init(Engine *_engine, Game *_game) { engine = _engine; game = _game; }
};

class Objects : public SimpleObjectList<Object> {
public:
	virtual void onUpdate() override;
	virtual void draw (const GraphicsContext &gc, Room *room, int cx, int cy, int cw, int ch);
};

#endif
