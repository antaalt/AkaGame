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
	cfg.graphic.api = aka::gfx::GraphicAPI::Vulkan;
	cfg.platform.width = 1280;
	cfg.platform.height = 720;
	cfg.platform.name = "Game";
	cfg.platform.icon.bytes;
	cfg.platform.icon.size;// = aka::Image::load(aka::ResourceManager::path("textures/icon/icon.png")); // TODO do not flip here
	cfg.app = &game;

	aka::Application::run(cfg);

	return 0;
}