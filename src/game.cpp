#include <assert.h>
#include "engine.h"
#include "color.h"
#include <math.h>
#include <list>
#include <algorithm>
#include "game.h"
#include "engine.h"
#include "anim.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "util.h"
#include <map>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <isogrid.h>
#include "text.h"
#include "button.h"
#include "resources.h"
#include "mainloop.h"
#include <math.h>
#include "DrawStrategy.h"
#include "map2d.h"
#include <iomanip>

using namespace std;

enum Action { TOGGLE_DEBUGMODE, CANCEL, PAUSE_MENU, QUIT, RESUME };

bool debugMode = false;

class GameImpl : public Game
{
	Engine *parent;

public:
	GameImpl(Engine *parent) : parent(parent) {
	}

	virtual ~GameImpl() {
	}

	virtual void init() override
	{
		Resources *res = parent->getResources();

		add (ClearScreen::build(BLACK).get());
		add (make_shared<Input>(ALLEGRO_KEY_F5, 0, TOGGLE_DEBUGMODE));
		add (make_shared<Input>(ALLEGRO_KEY_ESCAPE, 0, CANCEL));
		add (make_shared<Input>(ALLEGRO_KEY_SPACE, ALLEGRO_KEY_ENTER, PAUSE_MENU));

		initGame();
	}


	virtual bool onHandleMessage(ComponentPtr src, int event) override
	{
		if (State::onHandleMessage(src, event)) return true;
		switch (event)
		{
		case QUIT:
			pushMsg(Engine::MSG_MAIN_MENU);
			break;
		case TOGGLE_DEBUGMODE:
			debugMode = !debugMode;
			break;
		default:
			return false;
		}
		return true;
	}

	virtual void onUpdate() override {
	}
private:

	void initGame() {
	}

};

shared_ptr<Game> Game::newInstance(Engine *parent)
{
	return make_shared<GameImpl>(parent);
}
