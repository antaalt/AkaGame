#include "GameApp.h"
#include "EditorApp.h"

int main()
{
#if 0
	aka::GameApp game;
#else
	aka::EditorApp game;
#endif

	aka::Config cfg;
	cfg.width = 1280;
	cfg.height = 720;
	cfg.name = "Game";
	cfg.icon = aka::Image::load(aka::ResourceManager::path("textures/icon/icon.png")); // TODO do not flip here
	cfg.app = &game;

	aka::Application::run(cfg);

	return 0;
}