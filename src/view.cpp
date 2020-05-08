#include <assert.h>
#include "view.h"

void View::init (int numPlayers, int player)
{
	const int defaults[3][6] =
	{
		{64, 64, 512, 352, 64, 0},
		{0, 0, 312, 320, 0, 320},
		{328, 160, 312, 320, 320, 0}
	};
	camera_x = 0;
	camera_y = 0;
	
	int i = (numPlayers == 1 ? 0 : player + 1);
	
	left = defaults[i][0];
	top = defaults[i][1];
	width = defaults[i][2];
	height = defaults[i][3];
	status_x = defaults[i][4];
	status_y = defaults[i][5];
}
