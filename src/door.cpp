#include <assert.h>
#include "door.h"

Anim *Door::sprite;

Door::Door (Room *r) : Object (r), otherRoom (NULL), otherDoor (NULL)
{
	setAnim(sprite);
	setVisible(true);
	solid = true;
}

void Door::init(Resources *res)
{
	sprite = res->getAnim("door");
}

void Door::link(Door *door)
{
	assert (door);
	otherDoor = door;
	otherRoom = door->getRoom();
	assert (door->otherDoor == NULL); // we don't want to overwrite old links
	assert (door->otherRoom == NULL); 
	door->otherDoor = this;
	door->otherRoom = getRoom();
}

Anim *Teleport::sprite;

Teleport::Teleport (Room *r) : Object (r), otherRoom (NULL), otherTeleport (NULL)
{
	setAnim(sprite);
	setVisible(true);
	solid = true;
}

void Teleport::init(Resources *res)
{
	sprite = res->getAnim("teleport");
}

void Teleport::link(Teleport *teleport)
{
	assert (teleport);
	otherTeleport = teleport;
	otherRoom = teleport->getRoom();
	assert (teleport->otherTeleport == NULL); // we don't want to overwrite old links
	assert (teleport->otherRoom == NULL);
	teleport->otherTeleport = this;
	teleport->otherRoom = getRoom();
}
