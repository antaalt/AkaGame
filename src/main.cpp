#include "Game/Views/GameView.h"

int main()
{
	aka::Config cfg;
	cfg.width = 1280;
	cfg.height = 720;
	cfg.name = "Game";
	cfg.view = aka::View::create<aka::GameView>();

	aka::Application::run(cfg);

	return 0;
}