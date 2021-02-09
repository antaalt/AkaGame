#include "Game/Game.h"

int main()
{
	aka::Game app;

	aka::Config cfg;
	cfg.width = 1280;
	cfg.height = 720;
	cfg.name = "Game";
	cfg.app = &app;

	aka::Game::run(cfg);

	return 0;
}