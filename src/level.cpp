#include <assert.h>
#include "level.h"
#include <math.h>

#include "door.h"
#include "monster.h"
#include "player.h"
#include "util.h"
#include "tilemap.h"

using namespace std;
using namespace xdom;

Level *createTestLevel(RoomSet *roomSet, Resources *resources, Objects *objects, int monsterHp)
{
	Level *level = new Level();

	Room *temp[15];
	for (int i = 0; i < 14; ++i) { temp[i] = NULL; }
	temp[0] = new Room(objects, roomSet->findRoom(true, true, true, true, false), monsterHp);
	temp[1] = new Room(objects, roomSet->findRoom(true, true, false, true, false), monsterHp);
	temp[2] = new Room(objects, roomSet->findRoom(false, true, false, false, true), monsterHp);
	temp[3] = new Room(objects, roomSet->findRoom(false, false, true, true, false), monsterHp);
	temp[4] = new Room(objects, roomSet->findRoom(false, false, true, false, false), monsterHp);
	temp[5] = new Room(objects, roomSet->findRoom(true, true, false, false, false), monsterHp);
	temp[6] = new Room(objects, roomSet->findRoom(true, false, false, false, true), monsterHp);
	temp[7] = new Room(objects, roomSet->findRoom(false, true, true, false, false), monsterHp);
	temp[8] = new Room(objects, roomSet->findRoom(true, false, false, true, true), monsterHp);
	temp[9] = new Room(objects, roomSet->findRoom(false, false, true, false, false), monsterHp);
	temp[10] = new Room(objects, roomSet->findRoom(true, false, true, true, true), monsterHp);
	temp[11] = new Room(objects, roomSet->findRoom(true, false, false, true, false), monsterHp);
	temp[12] = new Room(objects, roomSet->findRoom(false, true, false, true, false), monsterHp);
	temp[13] = new Room(objects, roomSet->findRoom(false, true, true, false, false), monsterHp);
	for (int i = 0; i < 14; ++i) { assert (temp[i]); level->rooms.push_back (temp[i]); }
	
	temp[0]->linkDoor (temp[1], 0);
	temp[0]->linkDoor (temp[5], 1);
	temp[0]->linkDoor (temp[10], 2);
	temp[0]->linkDoor (temp[7], 3);
	temp[1]->linkDoor (temp[2], 0);
	temp[1]->linkDoor (temp[3], 3);
	temp[2]->linkTeleport (temp[6]);
	temp[3]->linkDoor (temp[4], 3);
	temp[5]->linkDoor (temp[6], 1);
	temp[7]->linkDoor (temp[8], 1);
	temp[8]->linkDoor (temp[9], 3);
	temp[8]->linkTeleport (temp[10]);
	temp[10]->linkDoor (temp[13], 0);
	temp[10]->linkDoor (temp[11], 2);
	temp[11]->linkDoor (temp[12], 0);
	temp[12]->linkDoor (temp[13], 3);

	// test level
	// 	level->rooms.push_back (new Room(objects, roomSet->findRoom(true, true, true, true, false), monsterHp));
	
	return level;
}

Level *createLevel(RoomSet *roomSet, Objects *objects, unsigned int numRooms, int monsterHp)
{
	const int up = 0x100;
	const int down = 0x200;
	const int left = 0x400;
	const int right = 0x800;
	const int start = 0x1000;
	const int used = 0x2000;
	const int teleport = 0x4000;
	const int side = 8;
	const int maxRooms = side * side;
	const int dr[4] = { -side, side, -1, 1 };
	const int df[4] = { up, down, left, right };
	const int dopp[4] = { 1, 0, 3, 2 };
	vector<int> rooms;
	int matrix[maxRooms];	
	if (numRooms > (unsigned int)maxRooms) numRooms = maxRooms;
	for (int i = 0; i < maxRooms; ++i) matrix[i] = 0;
	{
		int x = rand () % maxRooms;
		
		matrix[x] |= start | used;
		rooms.push_back (x);
	}
	
	while (rooms.size() < numRooms)
	{
		bool ok = false;
		int src;
		int dest;
		int d;
		// look for a room (src) with a connection (d) to an unoccupied space (dest).
		while (!ok)
		{
			int r = rand() % rooms.size();
			d = (rand() >> 4) % 4;
			src = rooms[r];
			dest = (src + dr[d]);
			while (dest < 0) dest += maxRooms;
			dest %= maxRooms;	
			if (matrix[dest] == 0)
			{
				ok = true;
			}			
		}
		
		// add door
		matrix[dest] |= used;
		rooms.push_back (dest);
		
		// create link
		int o = dopp[d]; // dir opposite to d
		matrix[src] |= df[d]; // flag for this dir
		matrix[dest] |= df[o]; // flag for opposite dir
	}
	
	// add some teleporters
	for (unsigned int i = 0; i < numRooms; i += 4) // one teleporter per 4 rooms
	{
		bool ok = false;
		int src = 0, dest = 0;
		while (!ok)
		{
			int rsrc = rand() % rooms.size();
			src = rooms[rsrc];
			if (!(matrix[src] & teleport))
			{
				int rdest = rand() % rooms.size();
				dest = rooms[rdest];
				if (dest != src && rsrc != rdest && 
						(!(matrix[dest] & teleport))
					)
				{
					ok = true;
				}
			}
		}
		assert (!(matrix[src] & teleport));
		assert (!(matrix[dest] & teleport));
		assert (src < maxRooms);
		assert (dest < maxRooms);
		assert (src >= 0);
		assert (dest >= 0);
		matrix[src] |= dest | teleport;
		matrix[dest] |= src | teleport;
	}
	
	Level *level = new Level();

	Room *matrix2[maxRooms];
	for (int i = 0; i < maxRooms; ++i) matrix2[i] = NULL;

	for (vector<int>::iterator i = rooms.begin(); i != rooms.end(); ++i)
	{
		int src = (*i);
		assert (matrix[src] > 0);
		RoomInfo *ri = roomSet->findRoom(
			matrix[src] & up, matrix[src] & down, 
			matrix[src] & left, matrix[src] & right, matrix[src] & teleport);
		if (!ri)
		{
			allegro_message ("Couldn't find roominfo for %i", matrix[src]);
		}
		assert (ri);
		
		Room *temp = new Room(objects, ri, monsterHp);
		assert (temp);
		
		matrix2[src] = temp;
		level->rooms.push_back(temp);
		
		for (int d = 0; d < 4; ++d)
		{
			int dest = (src + dr[d]);
			while (dest < 0) dest += maxRooms;
			dest %= maxRooms;	
			if ((matrix[src] & df[d]) && matrix2[dest])
			{
				assert (matrix[dest] > 0);
				// create door link
				matrix2[src]->linkDoor (matrix2[dest], d);
			}
		}
		if (matrix[src] & teleport)
		{
			int dest = matrix[src] & 0xFF;
			assert ((matrix[dest] & 0xFF) == src);
			if (matrix2[dest])
			{
				// create teleport link
				matrix2[src]->linkTeleport (matrix2[dest]);
			}
		}
	}
	
	return level;
}

RoomSet *RoomSet::loadFromFile (string filename, Resources *res)
{
	DomNode node = xmlParseFile(filename);
	
	vector<DomNode>::iterator n;
	n = node.children.begin();
	return RoomSet::loadFromXml (&(*n), res);
}

RoomSet *RoomSet::loadFromXml (DomNode *n, Resources *res)
{
	assert (n->name == "roomset");
	vector<DomNode>::iterator i;
	vector<DomNode>::iterator h;
	RoomSet *result = new RoomSet();
	
	for (h = n->children.begin(); h != n->children.end(); ++h)
	{
		assert (h->name == "room");
		RoomInfo ri;
		RoomInfo ri_tele;
		/* create two rooms, one with and one without teleporter */
		ri.map = res->getJsonMap(h->attributes["map"])->map;
		ri_tele.map = res->getJsonMap(h->attributes["map"])->map;

		for (int x = 0; x < ri.map->w; ++x)
			for (int y = 0; y < ri.map->h; ++y)
			{
				// get object data from layer 2.
				int tile = teg_mapget(ri.map, 2, x, y);
				int flag = ri.map->tilelist->tiles[tile].flags;

				if (flag >= 100)
				{
					ObjectInfo oi;
					oi.x = x;
					oi.y = y;
					switch (flag)
					{
					case 100:
						oi.type = ObjectInfo::DOOR;
						oi.doorDir = 0;
						ri.up = true; ri_tele.up = true;
						break;
					case 101:
						oi.type = ObjectInfo::DOOR;
						oi.doorDir = 3;
						ri.right = true; ri_tele.right = true;
						break;
					case 102:
						oi.type = ObjectInfo::DOOR;
						oi.doorDir = 1;
						ri.down = true; ri_tele.down = true;
						break;
					case 103:
						oi.type = ObjectInfo::DOOR;
						oi.doorDir = 2;
						ri.left = true; ri_tele.left = true;
						break;
					case 104:
						oi.type = ObjectInfo::MONSTER;
						oi.monsterType = 0;
						break;
					case 105:
						oi.type = ObjectInfo::MONSTER;
						oi.monsterType = 1;
						break;
					case 106:
						oi.type = ObjectInfo::MONSTER;
						oi.monsterType = 2;
						break;
					case 107:
						oi.type = ObjectInfo::MONSTER;
						oi.monsterType = 3;
						break;
					case 108:
						oi.type = ObjectInfo::BANANA;
						ri.bananas++;
						ri_tele.bananas++;
						break;
					case 109:
						oi.type = ObjectInfo::TELEPORT;
						ri_tele.teleport = true; /** add teleporter only to tele */
						break;
					case 110:
						oi.type = ObjectInfo::PLAYER;
						oi.pi = 0;
						ri.playerStart = true; ri_tele.playerStart = true;
						break;
					case 111:
						oi.type = ObjectInfo::PLAYER;
						oi.pi = 1;
						ri.playerStart = true; ri_tele.playerStart = true;
						break;
					default:
						assert (false); // wrong type
						break;
					}
					ri.objectInfo.push_back(oi);
					ri_tele.objectInfo.push_back(oi);
				}

		}
		result->rooms.push_back (ri);
		result->rooms.push_back (ri_tele);
	}
	return result;
}

RoomInfo *RoomSet::findRoom (bool up, bool down, bool left, bool right, bool teleport)
{
	RoomInfo *result = NULL;
	vector <RoomInfo>::iterator i;
	for (i = rooms.begin(); i != rooms.end(); ++i)
	{
		if (i->up == up && i->down == down && i->left == left && i->right == right && i->teleport == teleport)
		{
			result = &(*i);
			break;
		}	
	}
	return result;
}

Room::Room (Objects *o, RoomInfo *ri, int monsterHp) : roomInfo(ri), objects (o), map (NULL)
{
	doors[0] = NULL;
	doors[1] = NULL;
	doors[2] = NULL;
	doors[3] = NULL;
	teleport = NULL;
	map = ri->map;
	
	// add monsters, doors, etc. (but do not link doors yet)
	vector <ObjectInfo>::iterator i;
	for (i = ri->objectInfo.begin(); i != ri->objectInfo.end(); ++i)
	{
		switch (i->type)
		{
			case ObjectInfo::DOOR:
				{
					Door *d = new Door (this);
					d->setDir (i->doorDir);
					d->setLocation ((al_fixed)i->x * 32, (al_fixed)i->y * 32);
					objects->add (d);
					doors[i->doorDir] = d;
				}
				break;
			case ObjectInfo::TELEPORT:
				{
					Teleport *t = new Teleport (this);
					t->setLocation ((al_fixed)i->x * 32, (al_fixed)i->y * 32);
					objects->add (t);
					teleport = t;
				}
				break;
			case ObjectInfo::MONSTER:
				{
					Monster *m = new Monster (this, i->monsterType, monsterHp);
					m->setLocation ((al_fixed)i->x * 32, (al_fixed)i->y * 32);
					objects->add (m);
				}
				break;
			case ObjectInfo::BANANA:
				{
					Banana *b = new Banana (this);
					b->setLocation ((al_fixed)i->x * 32, (al_fixed)i->y * 32);
					objects->add (b);
				}
				break;
			case ObjectInfo::PLAYER:
				break;
		}
	}
}

void Room::linkDoor (Room *otherRoom, int dir)
{
	const int opposite[4] = {1, 0, 3, 2};
	int odir = opposite[dir];
	assert (doors[dir]);
	assert (otherRoom);
	assert (otherRoom->doors[odir]);
	doors[dir]->link(otherRoom->doors[odir]);
}

void Room::linkTeleport (Room *otherRoom)
{
	assert (teleport);
	assert (otherRoom);
	assert (otherRoom->teleport);
	teleport->link(otherRoom->teleport);	
}

Level::~Level()
{
	vector<Room *>::iterator i;
	for (i = rooms.begin(); i != rooms.end(); ++i)
	{
		delete (*i);
	}
}

int Level::getBananaCount()
{
	int result = 0;
	vector <Room *>::iterator i;
	for (i = rooms.begin(); i != rooms.end(); ++i)
	{
		result += (*i)->getBananaCount();
	}
	return result;
}

int Room::getBananaCount()
{
	assert (roomInfo);
	return roomInfo->bananas;
}