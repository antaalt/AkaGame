#pragma once

#include <Aka/Aka.h>
#include <map>

#include "../../GUI/EditorUI.h"
#include "../Resources.h"
#include "../Game.h"

namespace aka {

struct PauseGameEvent {
	bool pause = false;
};

class GameView : public View, EventListener<PauseGameEvent>
{
public:
	GameView();
	void onCreate() override;
	void onDestroy() override;
	void onFrame() override;
	void onUpdate(Time::Unit deltaTime) override;
	void onRender() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const PauseGameEvent& event) override;
private:
	EditorUI m_gui;
	// Rendering
	bool m_paused;
	Framebuffer::Ptr m_framebuffer;
	// Entity
	Game m_game;
};

}

