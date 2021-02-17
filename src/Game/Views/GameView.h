#pragma once

#include <Aka/Aka.h>
#include <map>

#include "../Resources.h"
#include "../Level.h"

namespace aka {

class GameView : public View
{
public:
	GameView(World &world);
	void onCreate() override;
	void onDestroy() override;
	void onFrame() override;
	void onUpdate(Router &router, Time::Unit deltaTime) override;
	void onRender() override;
private:
	// Rendering
	bool m_paused;
	Batch m_batch;
	Framebuffer::Ptr m_framebuffer;
	// Entity
	World &m_world;
	Entity m_cameraEntity;
	// Levels
	WorldMap m_map;
};

}

