#ifndef VIEW_H
#define VIEW_H

#include "tegel5.h"
#include "player.h"

class View
{
	public:
		void init (int numPlayers, int player);
		
		int camera_x, camera_y;
		int top, left, width, height;
		int status_x, status_y;
		Player *player;
		
		void draw(ALLEGRO_BITMAP *buffer);
};

#endif
