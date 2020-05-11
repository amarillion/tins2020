#include "game.h"
#include "container.h"
#include <allegro5/allegro.h>
#include "color.h"
#include <allegro5/allegro_font.h>
#include "util.h"
#include "mainloop.h"
#include "strutil.h"

using namespace std;

Game::Game(Engine *engine, Settings *_settings) : objects(), parent(engine), settings(_settings)
{
	player[0] = NULL;
	player[1] = NULL;
	level = NULL;
	roomSet = NULL;

	btnPause.setScancode(ALLEGRO_KEY_ESCAPE);

	messages = make_shared<Messages>();
	add(messages);
}

Game::~Game()
{
	if (level != NULL) delete level;
	objects.killAll();
	if (roomSet != NULL) delete roomSet;
}

// TODO: copy this function back to tegel
// TODO: merge with laundryDay->teg_draw_repeated
void Game::teg_partdraw (ALLEGRO_BITMAP *bmp, const TEG_MAP* map, int layer, int cx, int cy, int cw, int ch, int xview, int yview)
{
	int x, y;
    int tilex, tiley;

    al_set_target_bitmap (bmp);
    clip_with_transform(cx, cy, cx + cw, cy + ch);

    for (tilex = 0; tilex < map->w; tilex++)
    {
        for (tiley = 0; tiley < map->h; tiley++)
        {
			x = tilex * map->tilelist->tilew - xview;
			y = tiley * map->tilelist->tileh - yview;

            int i = teg_mapget(map, layer, tilex, tiley);
            if (i >= 0 && i < map->tilelist->tilenum)
            {
            	teg_drawtile(map->tilelist, i, x, y);
            }
        }
    }

    al_set_clipping_rectangle(0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp));
}

void Game::draw (const GraphicsContext &gc)
{
	al_set_target_bitmap(gc.buffer);
	al_clear_to_color (BLACK);
	//~ teg_draw (buffer, level, 0, camera_x, 0);
	//~ objects.draw(buffer, camera_x, 0);
	for (int i = 0; i < settings->numPlayers; ++i)
	{
		if (!ps[i].died)
		{
			TEG_MAP *map = view[i].player->getMap();
			teg_partdraw (gc.buffer, map, 0,
					view[i].left, view[i].top, view[i].width, view[i].height,
					view[i].camera_x - view[i].left, view[i].camera_y - view[i].top);
			teg_partdraw (gc.buffer, map, 1,
					view[i].left, view[i].top, view[i].width, view[i].height,
					view[i].camera_x - view[i].left, view[i].camera_y - view[i].top);
			GraphicsContext gc2;
			gc2.buffer = gc.buffer;
			gc2.xofst = view[i].camera_x - view[i].left;
			gc2.yofst = view[i].camera_y - view[i].top;
			objects.draw(gc2, view[i].player->getRoom(), view[i].left, view[i].top,
					view[i].width, view[i].height);
		}
		drawStatus (gc.buffer, view[i].status_x, view[i].status_y, &ps[i]);
	}
	int min = (gameTimer / 60000);
	int sec = (gameTimer / 1000) % 60;
	int csec = (gameTimer / 10) % 100;
	al_draw_textf (gamefont, WHITE, 0, 464, ALLEGRO_ALIGN_LEFT, "%02i:%02i:%02i", min, sec, csec);

	messages->draw(gc);
}

void Game::update()
{
	if (gameTimer % 1000 == 0) {
		if (gameTimer == 60000) {
				messages->showMessage("One minute remaining", Messages::RIGHT_TO_LEFT);
		}

		if (gameTimer == 30000) {
				messages->showMessage("Hurry up!", Messages::RIGHT_TO_LEFT);
		}

		if (gameTimer >= 1000 && gameTimer <= 10000) {
				messages->showMessage(string_format("%i", (gameTimer / 1000) - 1 ), Messages::RIGHT_TO_LEFT);
		}
	}

	messages->update();

	if (btnPause.justPressed())
	{
		pushMsg(Engine::MENU_PAUSE);
	}

	// add objects based on position
	gameTimer -= MainLoop::getMainLoop()->getLogicIntervalMsec();
	objects.update();
	bool gameover = true;
	int bananasGot = 0;

	for (int i = 0; i < settings->numPlayers; ++i)
	{
		// update camera
		int newx, newy;
		newx = player[i]->getx() - (view[i].width / 2);
		newy = player[i]->gety() - (view[i].height / 2);
// 				if (newx >= 0)
			view[i].camera_x = newx;
// 				if (newy >= 0)
			view[i].camera_y = newy;

		if (!ps[i].died) gameover = false;
		bananasGot += ps[i].bananas;
	}
	if (gameover)
	{
		gameover_message = "GAME OVER";
		pushMsg(Engine::GS_GAME_OVER);
	}
	if (gameTimer <= 0)
	{
		gameover_message = "TIME UP";
		pushMsg(Engine::GS_GAME_OVER);
	}
	if (bananaCount - bananasGot == 0)
	{
		doneLevel();
		nextLevel();
		pushMsg(Engine::E_NEXT_LEVEL);
		return;
	}
}

void Game::drawStatus (ALLEGRO_BITMAP *buffer, int x, int y, PlayerState *xps)
{
	int bananasGot = 0;
	for (int i = 0; i < settings->numPlayers; ++i) bananasGot += ps[i].bananas;
	al_draw_textf (gamefont, YELLOW, x + 8, y + 8, ALLEGRO_ALIGN_LEFT, "BANANAS %02i LEFT %02i", xps->bananas, bananaCount - bananasGot);
	al_draw_textf (gamefont, CYAN, x + 8, y + 24, ALLEGRO_ALIGN_LEFT, "KEYS %02i", xps->keys);
	al_draw_textf (gamefont, RED, x + 8, y + 40, ALLEGRO_ALIGN_LEFT, "HEALTH %03i MAX %03i", xps->hp, xps->hpMax);
	al_draw_textf (gamefont, WHITE, x + 8, y + 56, ALLEGRO_ALIGN_LEFT, "XP %05i", xps->xp);
}

void Game::initGame ()
{
	// initialize game and player stats.
	// to start a new game.
	currentLevel = 0;
	gameTimer = defaultGameTime;

	// create a level
	// TODO: dynamic
	ps[0] = PlayerState();
	ps[1] = PlayerState();

	monsterHp = defaultMonsterHp;
	initLevel();
}

void Game::initLevel()
{
	// initialize objects
	level = createLevel(roomSet, &objects, currentLevel + (settings->numPlayers == 1 ? 4 : 6), monsterHp);

	Room *startRoom = level->getStartRoom();

	player[0] = new Player (&ps[0], startRoom, 0);
	player[0]->setLocation ((al_fixed)120, (al_fixed)160);
	objects.add (player[0]);
	if (settings->numPlayers == 2)
	{
		player[1] = new Player (&ps[1], startRoom, 1);
		player[1]->setLocation ((al_fixed)160, (al_fixed)120);
		player[1]->setRoom (startRoom);
		objects.add (player[1]);
	}
	else
	{
		player[1] = NULL;
	}

 	view[0].init (settings->numPlayers, 0);
	view[0].player = player[0];
	view[1].init (2, 1);
	view[1].player = player[1];

	bananaCount = level->getBananaCount();

	messages->showMessage(string_format("Collect %i Bananas", bananaCount), Messages::RIGHT_TO_LEFT);
}

void Game::doneLevel()
{
	objects.killAll();
	if (level != NULL)
	{
		delete level;
		level = NULL;
	}
}

// called when exit found
void Game::nextLevel()
{
	// go on to next level
	currentLevel++;
	int max = 0;
	int maxPlayer = 0;
	int i;
	for (i = 0; i < settings->numPlayers; ++i)
	{
		if (ps[i].bananas > max)
		{
			maxPlayer = i;
			max = ps[i].bananas;
		}

	}
	ps[maxPlayer].hpMax += 25;
	for (i = 0; i < settings->numPlayers; ++i)
	{
		ps[i].hp = ps[i].hpMax;
		ps[i].bananas = 0;
	}
	gameTimer += gameTimeIncrease; // extra minute
	monsterHp += monsterHpIncrease;
	initLevel();
}

Player *Game::getNearestPlayer (Object *o)
{
	assert (o);
	if (settings->numPlayers == 2)
	{
		Room *r1, *r2;
		r1 = player[0]->getRoom();
		r2 = player[1]->getRoom();
		if (r1 == o->getRoom() && r2 == o->getRoom())
		{
			double dx1 = player[0]->getx() - o->getx();
			double dy1 = player[0]->gety() - o->gety();
			double dx2 = player[1]->getx() - o->getx();
			double dy2 = player[1]->gety() - o->gety();
			if (dx1 * dx1 + dy1 * dy1 < dx2 * dx2 + dy2 * dy2)
			{
				return player[1];
			}
			else
			{
				return player[0];
			}
		}
		else if (r1 == o->getRoom())
		{
			return player[0];
		}
		else if (r2 == o->getRoom())
		{
			return player[1];
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		if (player[0]->getRoom() == o->getRoom())
		{
			return player[0];
		}
		else
		{
			return NULL;
		}
	}
}

void Game::init (Resources *resources)
{
	roomSet = RoomSet::loadFromFile("data/rooms.xml", resources);
	gamefont = resources->getFont("builtin_font");
	messages->setFont(resources->getFont("SpicyRice-Regular_48"));
}
