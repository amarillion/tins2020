#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <list>
#include <map>
#include "tegel5.h"
#include "resources.h"
#include "object.h"
#include "dom.h"

class Object;
class Objects;
class Door;
class Teleport;

class ObjectInfo
{
	public:
		enum ObjectInfoType {DOOR, TELEPORT, MONSTER, BANANA, PLAYER};
		ObjectInfoType type;
		
		int doorDir; // only for DOOR
		int x;
		int y;
		int monsterType; // only for MONSTER
		int pi; // only for PLAYER
};

class RoomInfo
{
	public:
		RoomInfo() : objectInfo(), map(NULL), 
			up(false), down(false), left(false), right(false), 
			teleport(false), bananas(0), playerStart(false) {}
		std::vector <ObjectInfo> objectInfo;
		TEG_MAP *map;
		bool up;
		bool down;
		bool left;
		bool right;
		bool teleport;
		int bananas;
		bool playerStart;
};

// container for all rooms
class RoomSet
{
	private:
		static RoomSet *loadFromXml (xdom::DomNode *n, Resources *res);
	public:
		std::vector<RoomInfo> rooms;		
		static RoomSet *loadFromFile (std::string filename, Resources *res);
		RoomInfo *findRoom (bool up, bool down, bool left, bool right, bool teleport);
};

class Room
{
		RoomInfo *roomInfo;
		Objects *objects; // global object container
		
		Door *doors[4]; // list of Doors in this room
 		Teleport *teleport;
	public:		
		Room(Objects *o, RoomInfo *ri, int monsterHp);
		TEG_MAP *map;
		int getStartX (int pi); // get start location of player pi
		int getStartY (int pi);
		void linkDoor (Room *otherRoom, int dir, bool reverse = true);
		void linkTeleport (Room *otherRoom, bool reverse = true);
		int getBananaCount();
};

class Level
{
	public:
		std::vector <Room *> rooms;
		Room *getStartRoom() { return (*(rooms.begin())); }
		Level() : rooms() {}
		~Level();
		int getBananaCount();
};

Level *createTestLevel(RoomSet *roomSet, Resources *resources, Objects *objects, int monsterHp);
Level *createLevel(RoomSet *roomSet, Objects *objects, unsigned int numRooms, int monsterHp);
Level* createLevel2(RoomSet *roomSet, Objects *objects, unsigned int numRooms, int monsterHp);

#endif
