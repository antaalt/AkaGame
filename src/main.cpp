#include "GameApp.h"
#include "EditorApp.h"

int main()
{
	//aka::GameApp game;
	aka::EditorApp game;

	aka::Config cfg;
	cfg.width = 1280;
	cfg.height = 720;
	cfg.name = "Game";
	cfg.app = &game;

	aka::Application::run(cfg);

	return 0;
}