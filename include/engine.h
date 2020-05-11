#ifndef ENGINE_H
#define ENGINE_H

#include <allegro5/allegro.h>
#include "object.h"
#include <tegel5.h>
#include "input.h"
#include "settings.h"
#include "resources.h"
#include "view.h"
#include "level.h"
#include "menubase.h"
#include "container.h"
#include "sound.h"

class Player;
class Game;

class Engine : public Container
{
public:
	Settings *getSettings() { return &settings; }
	Resources* getResources () { return &resources; }

	// menu states
	enum { E_NONE, E_START_OR_RESUME, E_NEXT_LEVEL, MENU_SOUND, MENU_PLAYER_NUM, MENU_KEYS_1, MENU_KEYS_2, E_QUIT, MENU_MAIN, COVER,
		GS_PLAY, MENU_PAUSE, GS_DONE, GS_GAME_OVER, E_TOGGLE_FULLSCREEN};
private:
	Resources resources;
	Settings settings;
	Input btnScreenshot;
#ifdef DEBUG
	Input btnAbort;
	Input btnDebugMode;
#endif
	bool debug;
	
	std::shared_ptr<Game> game;
	
	//TODO: move to TEGEL
	void teg_partdraw_rle (ALLEGRO_BITMAP *bmp, const TEG_MAP* map, int layer, int x, int y, int w, int h, int xview, int yview);

	ALLEGRO_FONT *gamefont;
	void showIntro();

public:
	Engine ();
	int init(); // call once during startup
	virtual void onUpdate () override;
	
	bool isDebug () { return debug; }

	int getCounter ();
		
	void playSample (ALLEGRO_SAMPLE *s);
	void playMusic (const char *id);
	void stopMusic ();

	Input* getInput(int p);	
	// AL_DUH_PLAYER *dp; // TODO

private:
	bool isResume;

	std::shared_ptr<ToggleMenuItem> miPlayerNum;
	std::shared_ptr<ToggleMenuItem> miSound;
	std::shared_ptr<ActionMenuItem> miStart;

	MenuScreenPtr mMain;
	MenuScreenPtr mKeys[2];

	void initMenu ();
public:
	virtual void handleMessage(ComponentPtr src, int code) override;

	void initStart();
	void initResume();
};

#endif
