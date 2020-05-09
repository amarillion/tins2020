#ifndef DOOR_H
#define DOOR_H

#include "object.h"
#include "color.h"

class Door : public Object
{	
	public:
		Door(Room *r);
		void link (Door *_door, bool reverse = true); // links these two doors
		Room *otherRoom;
		Door *otherDoor;
		static void init(Resources *res);
		
		static Anim *sprite;		
		virtual void handleCollission(ObjectBase *o) {};
		virtual int getType () { return OT_DOOR; }
};

class Teleport : public Object
{	
	public:
		Teleport (Room *r);
		void link (Teleport *_teleport, bool reverse = true); 
		Room *otherRoom;
		Teleport *otherTeleport;
		static void init(Resources *res);
		
		static Anim *sprite;	
		virtual void handleCollission(ObjectBase *o) {};
		virtual int getType () { return OT_TELEPORT; }
};

#endif
