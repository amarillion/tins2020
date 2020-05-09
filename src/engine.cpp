#include "color.h"
#include "door.h"
#include "engine.h"
#include "monster.h"
#include "object.h"
#include "player.h"
#include "screenshot.h"
#include <assert.h>
#include <stdio.h>
#include "mainloop.h"
#include "component.h"
#include <memory>
#include "game.h"
#include "keymenuitem.h"
#include "util.h"
#include "mainloop.h"
#include "text.h"
#include "DrawStrategy.h"
#include "strutil.h"
#include "tilemap.h"

using namespace std;

int Engine::getCounter () 
{ 
	return MainLoop::getMainLoop()->getCounter();
}

Engine::Engine() : resources(), settings()
{
	debug = false;
	isResume = false;

	btnScreenshot.setScancode (ALLEGRO_KEY_F12);
#ifdef DEBUG
	btnAbort.setScancode (ALLEGRO_KEY_F10);
	btnDebugMode.setScancode (ALLEGRO_KEY_F11);
#endif

	game = make_shared<Game>(this, &settings);
	add(game, Container::FLAG_SLEEP);
}

Input* Engine::getInput(int p)
{ 
	return settings.button[p];
}

int Engine::init()
{
	settings.getFromArgs (MainLoop::getMainLoop()->getOpts());
	settings.getFromConfig(MainLoop::getMainLoop()->getConfig());
	initKeyboard(); // install custom keyboard handler

	const char * directions[] = { "up", "down", "left", "right" };
	Anim::setDirectionModel (make_shared<DirectionModel>(directions, 4));

	if (!(
		resources.addFiles("data/*.MOD") &&
		resources.addFiles("data/*.png") &&
		resources.addFiles("data/anim.xml") &&
		resources.addFiles("data/*.ttf") &&
		resources.addFiles("data/*.tll") &&
		resources.addFiles("data/*.wav") &&
		resources.addFiles("data/*.tga")
		))
	{
		allegro_message ("Could not load all resources with error %s!", resources.getErrorMsg());
		return 1;
	}

	vector<string> levels = {
		"bar1", "bar2", "bdoor", "bend1", "bend2", "bend3", "bend4", 
		"cross", "enclosed", "ldoor", "rdoor", "tdoor", "tee1", "tee2", "tee3", "tee4"
	};

	//TODO: store tileset reference in map itself.
	for (auto key : levels) {

		auto filename = string_format("data/%s-tiled.json", key.c_str());
		int result = resources.addJsonMapFile(key, filename, "tiles1");
		if (!result)
		{
			allegro_message ("Could not load all resources!\n%s", resources.getErrorMsg());
			return 0;
		}
		auto amap = resources.getJsonMap(key);
		assert (amap->map->tilelist);
	}

	ObjectBase::init (&debug, MainLoop::getMainLoop());
	Object::init (this, game.get());
	Player::init(&resources);
	Bullet::init(&resources);
	Door::init(&resources);
	Monster::init(&resources);
	Banana::init(&resources);
	Teleport::init(&resources);

	game->init(&resources);
	gamefont = resources.getFont("fixed_font");

 	srand(time(0));

 	initMenu();
 	handleMessage(nullptr, MENU_MAIN);

	return 0;
}

 /*
  // TODO: clean up

		case MENU: // run menu
			menu.update();
			result = menu.getState();
			switch (result)
			{
				case 0: state = QUIT; break; // quit
				case 1:
					engine.initGame();
					playMusic(resources.getDuh("SYREEN"));
					state = GAME;
					break; // start new game
				case 2:
					engine.resume();
					playMusic(resources.getDuh("SYREEN"));
					state = GAME;
					break; // resume game
				case -1: break; // continue showing menu
				default: assert (false); // shouldn't occur
			}
			break;
		case QUIT: // quit
			kill();
			break;
		case GAME: // game is playing, no matter if at intro screen or play screen
			engine.update();
			{
				Engine::GameState gs = engine.getState();
				switch (gs)
				{
					case Engine::GS_INTRO:
					case Engine::GS_OUTRO:
					case Engine::GS_PLAY:
						break; // do nothing
					case Engine::GS_MENU_RESUME: // resume menu
						menu.initResume();
						stopMusic();
						state = MENU;
						break;
					case Engine::GS_DONE: // go back to start menu
						menu.initStart();
						stopMusic();
						state = MENU;
						break;
					default:
						assert (false);
				}
			}
			break;
		default:
			assert (false); // shouldn't occur
	}

	*/

void Engine::showIntro()
{
	MainLoop::getMainLoop()->playMusic(resources.getMusic("SYREEN"));
	ContainerPtr intro = make_shared<Container>();
	add(intro);
	intro->add(ClearScreen::build(BLACK).get());
	intro->add(Text::buildf(WHITE, "LEVEL %i", game->getCurrentLevel() + 1)
		.center().get());
	if (game->getCurrentLevel() > 0)
	{
		intro->add(Text::buildf(WHITE, "EXTRA TIME", game->getCurrentLevel() + 1)
				.xy(MAIN_WIDTH / 2, MAIN_HEIGHT * 2 / 3).get()); /* TODO .blink(100) */
	}

	intro->setTimer (50, MSG_KILL);
	setFocus(intro);
	setTimer(50, GS_PLAY);
}

void Engine::handleMessage(ComponentPtr src, int code)
{
	switch (code)
	{
	case GS_GAME_OVER:
		{
			game->doneLevel();

			ContainerPtr intro = make_shared<Container>();
			add(intro);
			intro->add(ClearScreen::build(BLACK).get());
			intro->add(Text::buildf(WHITE, "%s", game->gameover_message.c_str())
					.center().get());
			initStart();
			intro->setTimer (50, MSG_KILL);
			setFocus(intro);
			setTimer(50, MENU_MAIN);
		}
		break;
	case GS_PLAY:
		setFocus (game);
		break;
	case E_TOGGLE_FULLSCREEN:
		MainLoop::getMainLoop()->toggleWindowed();
		break;
	case E_START_OR_RESUME:
	{
		if (!isResume)
		{
			game->initGame();
			showIntro();
		}
		else
		{
			setFocus(game);
		}
	}
	break;
	case E_NEXT_LEVEL:
	{
		showIntro();
	}
	break;
	case MENU_PAUSE:
		initResume();
		break;
	case MENU_SOUND:
		MainLoop::getMainLoop()->setSoundOn (!MainLoop::getMainLoop()->isSoundOn());
		break;
	case MENU_PLAYER_NUM:
		if (!isResume)
		{
			settings.numPlayers = 3 - settings.numPlayers;
			set_config_int (MainLoop::getMainLoop()->getConfig(), "fole1", "numplayers", settings.numPlayers);
		}
		break;
	case MENU_KEYS_1:
		setFocus (mKeys[0]);
		break;
	case MENU_KEYS_2:
		setFocus (mKeys[1]);
		break;
	case E_QUIT:
		pushMsg(MSG_CLOSE);
		break;
	case MENU_MAIN:
		setFocus (mMain);
		break;
	}
}

void Engine::onUpdate ()
{
	//TODO: implement these buttons as children of mainloop that send action events on press
	if (btnScreenshot.justPressed())
	{
		screenshot();
	}

#ifdef DEBUG
	if (btnDebugMode.justPressed())
	{
		debug = !debug;
	}
	if (btnAbort.justPressed())
	{
		pushMsg(E_QUIT);
	}
#endif
}


void Engine::initMenu()
{
	//TODO
	// setMargins (160, 100);

	//TODO
	//sound_cursor = resources->getSample ("pong");
	//sound_enter = resources->getSample ("mugly2");

//	ALLEGRO_BITMAP *cover = resources.getBitmap("cover");

	//TODO: used to be font "Metro"
	//menufont = resources->getAlfont("Vera", 24);
	sfont = resources.getFont("Vera_24");

	miSound = make_shared<ToggleMenuItem>(MENU_SOUND, "Sound is on", "Sound is off", "press enter to turn sound on or off");
	miSound->setToggle(MainLoop::getMainLoop()->isSoundOn());
	miSound->setEnabled(MainLoop::getMainLoop()->isSoundInstalled());

	miPlayerNum = make_shared<ToggleMenuItem>(MENU_PLAYER_NUM,
			"2 Player mode", "1 Player mode", "press enter to change number of players");
	miPlayerNum->setToggle(settings.numPlayers == 2);

	miStart = make_shared<ActionMenuItem>(E_START_OR_RESUME, "Start game", "");
	mMain = MenuBuilder(this, NULL)
		.push_back (miStart)
		.push_back (miSound)
		.push_back (miPlayerNum)
		.push_back (make_shared<ActionMenuItem>(MENU_KEYS_1, "Configure player 1 keys", ""))
		.push_back (make_shared<ActionMenuItem>(MENU_KEYS_2, "Configure player 2 keys", ""))
		.push_back (make_shared<ActionMenuItem>(E_TOGGLE_FULLSCREEN, "Toggle Fullscreen", "Switch fullscreen / windowed mode"))
		.push_back (make_shared<ActionMenuItem>(E_QUIT, "Quit", ""))
		.build();
	mMain->add(ClearScreen::build(BLACK).get(), FLAG_BOTTOM);
	mMain->setMargin(160, 80);
//	mMain->add(BitmapComp::build(cover).xy((MAIN_WIDTH - al_get_bitmap_width(cover)) / 2, 80).get());

	for (int i = 0; i < 2; ++i)
	{
		ALLEGRO_CONFIG *config = MainLoop::getMainLoop()->getConfig();
		mKeys[i] = MenuBuilder (this, NULL)
			.push_back (make_shared<KeyMenuItem> ("left", config_keys[i][0], settings.getInput(i)[btnLeft], config))
			.push_back (make_shared<KeyMenuItem> ("right", config_keys[i][1], settings.getInput(i)[btnRight], config))
			.push_back (make_shared<KeyMenuItem> ("down", config_keys[i][2], settings.getInput(i)[btnDown], config))
			.push_back (make_shared<KeyMenuItem> ("up", config_keys[i][3], settings.getInput(i)[btnUp], config))
			.push_back (make_shared<KeyMenuItem> ("action", config_keys[i][4], settings.getInput(i)[btnAction], config))
			.push_back (make_shared<KeyMenuItem> ("alt", config_keys[i][5], settings.getInput(i)[btnAlt], config))
			.push_back (make_shared<ActionMenuItem> (MENU_MAIN, "Return", "Return to main menu"))
			.build();
		mKeys[i]->add(ClearScreen::build(BLACK).get(), FLAG_BOTTOM);
		mKeys[i]->setMargin(160, 80);
//		mKeys[i]->add(BitmapComp::build(cover).xy((MAIN_WIDTH - al_get_bitmap_width(cover)) / 2, 80).get());
	}
}


void Engine::initStart()
{
	setFocus (mMain);
	miStart->setText("Start");
	miPlayerNum->setEnabled(true);
	isResume = false;
}

void Engine::initResume()
{
	setFocus (mMain);
	miStart->setText("Resume");
	miPlayerNum->setEnabled(false);
	isResume = true;
}
