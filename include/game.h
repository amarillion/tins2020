#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "data.h"

class Engine;

extern bool debugMode;

class Game : public State
{
public:
	virtual void init() = 0;
	static std::shared_ptr<Game> newInstance(Engine *parent);
};

#endif
