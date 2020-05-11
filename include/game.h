#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "object.h"
#include "engine.h"
#include "player.h"
#include "messages.h"

class Game : public State
{
private:
	RoomSet *roomSet;
	Objects objects;
	Engine *parent;
	Player *player[2];
	PlayerState ps[2];
	View view[2]; // a view for each player
	int gameTimer;
	Settings *settings;
	Level *level;
	int monsterHp;

	static const int defaultGameTime = 180000; // 180 sec = 3 min
	static const int defaultMonsterHp = 5;
	static const int monsterHpIncrease = 2;
	static const int gameTimeIncrease = 60000;

	int bananaCount;

	unsigned int currentLevel; // index of current Level

	//TODO: move to TEGEL
	void teg_partdraw (ALLEGRO_BITMAP *bmp, const TEG_MAP* map, int layer, int cx, int cy, int cw, int ch, int xview, int yview);

	void drawStatus (ALLEGRO_BITMAP *buffer, int x, int y, PlayerState *ps);
	void nextLevel(); // called when player reaches exit
	void initLevel (); // for beginning a new level

	ALLEGRO_FONT *gamefont;
	Input btnPause;
public:
	//TODO: make private
	std::string gameover_message;
	std::shared_ptr<Messages> messages;
	
	void addTime(int amount) { gameTimer += amount; messages->showMessage("Extra time", Messages::RIGHT_TO_LEFT); }

	void doneLevel (); // clean up objects etc.
	void initGame ();
	int getCurrentLevel () { return currentLevel; }
	Objects *getObjects() { return &objects; }
	Game (Engine *engine, Settings *_settings);
	virtual ~Game();
	virtual void draw(const GraphicsContext &gc) override;
	virtual void update() override;
	Player *getNearestPlayer (Object *o);
	void init(Resources *resources);
};

#endif
