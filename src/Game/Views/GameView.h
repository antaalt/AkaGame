#pragma once

#include <Aka/Aka.h>
#include <map>

#include "../../GUI/EditorUI.h"
#include "../Resources.h"
#include "../Level.h"

namespace aka {

class GameView : public View
{
public:
	GameView();
	void onCreate() override;
	void onDestroy() override;
	void onFrame() override;
	void onUpdate(Time::Unit deltaTime) override;
	void onRender() override;
	void onResize(uint32_t width, uint32_t height) override;
private:
	EditorUI m_gui;
	// Rendering
	bool m_paused;
	Batch m_batch;
	Framebuffer::Ptr m_framebuffer;
	// Entity
	World m_world;
	Entity m_cameraEntity;
	// Levels
	WorldMap m_map;
};

}

