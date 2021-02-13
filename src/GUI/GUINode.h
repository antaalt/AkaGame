#pragma once

#include "../Game/Resources.h"

#define USE_IMGUI

#include <sstream>
#if defined(USE_IMGUI)
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#if defined(AKA_USE_OPENGL)
#include <backends/imgui_impl_opengl3.h>
#endif
#if defined(AKA_USE_D3D11)
#include <backends/imgui_impl_dx11.h>
#endif
#endif

#if defined(AKA_PLATFORM_WINDOWS)
#define STR_CPY(dest, destSize, src) strcpy_s(dest, destSize, src);
#else
#define STR_CPY(dest, destSize, src) strcpy(dest, src);
#endif

namespace aka {

class GUIWidget
{
public:
	virtual void update() {}
	virtual void draw(World& world) {}
};

class GUI
{
public:
	GUI() {}
	~GUI() {
		for (GUIWidget* widget : m_widgets)
			delete widget;
	}
	void initialize();
	void destroy();
	void update() {
		for (GUIWidget *widget : m_widgets)
			widget->update();
	}
	void draw(World& world) {
		if (m_visible)
			for (GUIWidget* widget : m_widgets)
				widget->draw(world);
	}
	void frame();
	void render();
	bool isVisible() { return m_visible; }
	void setVisible(bool visible) { m_visible = visible; }

	void add(GUIWidget* widget) { m_widgets.push_back(widget); }

	bool focused() const { const ImGuiIO& io = ImGui::GetIO(); return io.WantCaptureMouse; }
private:
	bool m_visible = true;
	std::vector<GUIWidget*> m_widgets;
};

}