#include <assert.h>
#include "mainloop.h"
#include "engine.h"

const int GAME_WIDTH=640;
const int GAME_HEIGHT=480;
using namespace std;

int main(int argc, const char *const *argv)
{
	auto mainloop = MainLoop();
	auto engine = make_shared<Engine>();

	mainloop
		.setEngine(engine)
		.setAppName("flowpow")
		.setTitle("Fole & Raul go Flower Power!")
		.setConfigFilename("flowpow.cfg")
		.setLogicIntervalMsec(20)
		.setPreferredGameResolution(GAME_WIDTH, GAME_HEIGHT);
		
	if (!mainloop.init(argc, argv) && !engine->init())
	{
		mainloop.run();
	}

	return 0;
}
