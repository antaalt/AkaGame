#pragma once 
#include "../EditorApp.h"

#include "../Game/Views/GameView.h"

namespace aka {

class MenuWidget : 
	public EditorWidget,
	EventListener<PauseGameEvent>
{
public:
	MenuWidget();
	void update(World &world) override;
	void draw(World& world) override;
	void onReceive(const PauseGameEvent& event) override;
private:
	// TODO request this from backend
	bool m_pause = false;
	bool m_vsync;
	bool m_fullscreen;
};

};