#pragma once 

#include <Aka/Aka.h>

#include "../GUI/GUINode.h"

namespace aka {

struct Views {
	static const ViewID menu;
	static const ViewID game;
	static const ViewID end;
};

class Game : public Application
{
public:
	void initialize() override;
	void destroy() override;
	void start() override;
	void update(Time::Unit deltaTime) override;
	void frame() override;
	void render() override;
	void present() override;
	void end() override;
private:
	// UI
	GUI m_gui;
	// ECS
	World m_world;
	// Routing
	View* current;
	Router router;
};

};