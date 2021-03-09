#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"
#include "../Level.h"

namespace aka {

struct UIStateComponent {
	bool hovered;
	bool active;
};

struct UIButtonComponent {
	Font* font;
	std::string text;
	float padding;
	color4f colorText;
	color4f colorButton;
	color4f colorButtonHovered;
	color4f colorButtonActive;
	int32_t layer;
	bool hovered; // TODO move state in UIComponent, available for every ui.
	bool active;

	std::function<void(const input::Position&)> callback;
};

struct UIImageComponent {
	Texture::Ptr texture;
	int32_t layer;
};

class UISystem : public System
{
public:
	void update(World& world, Time::Unit deltaTime) override;
	void draw(World& world, Batch& batch) override;
};

class MenuView : public View {
public:
	void onCreate() override;
	void onDestroy() override;
	void onUpdate(Time::Unit dt) override;
	void onRender() override;
	void onResize(uint32_t width, uint32_t height) override;
private:
	World m_world;
	Batch m_batch;
};

};