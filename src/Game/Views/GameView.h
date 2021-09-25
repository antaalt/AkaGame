#pragma once

#include <Aka/Aka.h>
#include <map>

#include "../Resources.h"
#include "../Game.h"

namespace aka {

class GameView : public View
{
public:
	void onCreate() override;
	void onDestroy() override;
	void onFixedUpdate(Time::Unit deltaTime) override;
	void onUpdate(Time::Unit deltaTime) override;
	void onRender() override;
	void onResize(uint32_t width, uint32_t height) override;
	Game& game();
private:
	Framebuffer::Ptr m_framebuffer;
	Game m_game;
};

}

