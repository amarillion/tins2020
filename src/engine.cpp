#include "engine.h"
#include "game.h"
#include "resources.h"
#include "util.h"
#include "mainloop.h"
#include "button.h"
#include "DrawStrategy.h"
#include "text.h"
#include "color.h"
#include "updatechecker.h"
#include "text.h"
#include "version.inc"
#include "input.h"

using namespace std;

const int DIRNUM = 4;
const char *DIRECTIONS[DIRNUM] = { "n", "e", "s", "w" };

class EngineImpl : public Engine
{
private:
	shared_ptr<Game> game;
	Resources resources;
	shared_ptr<State> menu;
	shared_ptr<UpdateChecker> updates;

public:
	EngineImpl() : game(Game::newInstance(this)), resources(), menu()
	{
	}

	virtual Resources* getResources() override { return &resources; }

	virtual int init() override
	{
		Anim::setDirectionModel (make_shared<DirectionModel>(DIRECTIONS, DIRNUM));
		srand(time(0));

/*
		if (!(
			resources.addFiles("data/*.ttf") &&
			resources.addFiles("data/*.png") &&
			resources.addFiles("data/*.xml") &&
			resources.addFiles("data/*.ogg") &&
			resources.addFiles("data/*.xm") &&
			resources.addFiles("data/*.wav")
			))
		{
			allegro_message ("Error while loading resources!\n%s", resources.getErrorMsg());
			return 1;
		}
*/
		add(game, FLAG_SLEEP);
		initMenu();
		setFocus(menu);

		game->init();
		// startMusic();

		ALLEGRO_PATH *localAppData = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
		string cacheDir = al_path_cstr(localAppData, ALLEGRO_NATIVE_PATH_SEP);
		al_destroy_path(localAppData);

		updates = UpdateChecker::newInstance(cacheDir, APPLICATION_VERSION, MSG_QUIT);

	 	add (updates, Container::FLAG_SLEEP);

	 	updates->start_check_thread("tins2020", "en");

		return 0;
	}

	virtual void done () override
	{
		updates->done();
	}

	// void startMusic()
	// {
	// 	MainLoop::getMainLoop()->playMusic(resources.getMusic("Boterham_happy"));
	// }

	void initMenu()
	{
		setFont(resources.getFont("builtin_font"));

		int buttonw = 200;
		int buttonh = 40;
		int yco = 50;
		int xco = 0; // from center
		int margin = 10;

		menu = make_shared<State>();

		menu->add(ClearScreen::build(BLACK).get());

		menu->add(Text::build(RED, ALLEGRO_ALIGN_CENTER, "TINS 2020").layout(Layout::LEFT_TOP_RIGHT_H, 0, yco, 0, 80).get());

		yco += 100;

		menu->add(Button::build(MSG_PLAY, 			"PLAY").layout(Layout::CENTER_TOP_W_H, xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;
		menu->add(Button::build(MSG_TOGGLE_WINDOWED,"WINDOWED ON/OFF").layout(Layout::CENTER_TOP_W_H, xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;
		menu->add(Button::build(MSG_TOGGLE_MUSIC,	"MUSIC ON/OFF").layout(Layout::CENTER_TOP_W_H, xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;

		//TODO
//		menu->add(Button::build(MSG_CREDITS, 		"CREDITS").layout(Layout::CENTER_TOP_W_H, xco, yco, buttonw, buttonh).get());
//		yco += buttonh + margin;

		menu->add(Button::build(MSG_QUIT_BUTTON, 	"QUIT").layout(Layout::CENTER_TOP_W_H, xco, yco, buttonw, buttonh).get());

		menu->add(Text::build(GREY, ALLEGRO_ALIGN_RIGHT, "v" APPLICATION_VERSION)
			.layout(Layout::RIGHT_BOTTOM_W_H, 4, 4, 200, 28).get());

		add (menu);
	}

	virtual void handleMessage(ComponentPtr src, int code) override
	{
		switch (code)
		{
		case MSG_MAIN_MENU:
			setFocus(menu);
			break;
		case MSG_QUIT:
			pushMsg(MSG_CLOSE);
			break;
		case MSG_QUIT_BUTTON:
			setFocus(updates);
			break;
		case MSG_PLAY:
			setFocus(game);
			break;
		case MSG_TOGGLE_WINDOWED:
			MainLoop::getMainLoop()->toggleWindowed();
			break;
		case MSG_TOGGLE_MUSIC:
		{
			bool enabled = MainLoop::getMainLoop()->isMusicOn();
			enabled = !enabled;
			MainLoop::getMainLoop()->setMusicOn(enabled);
			// if (enabled) startMusic();
		}
			break;
		case MSG_CREDITS:
			// TODO
			break;
		}
	}
};

shared_ptr<Engine> Engine::newInstance()
{
	return make_shared<EngineImpl>();
}
