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

namespace aka {

class EditorUI final
{
public:
	struct Widget {
		virtual ~Widget() {}
		virtual void initialize() {}
		virtual void destroy() {}
		virtual void update(World& world) {}
		virtual void draw(World& world) {}
	};
public:
	EditorUI();
	~EditorUI();

	template <typename T, typename... Args>
	void attach(Args&&... args);
	void initialize();
	void destroy();
	void update(World& world);
	void draw(World& world);
	void frame();
	void render();
	bool isVisible() { return m_visible; }
	void setVisible(bool visible) { m_visible = visible; }

	bool focused() const { const ImGuiIO& io = ImGui::GetIO(); return io.WantCaptureMouse; }
private:
	bool m_visible = true;
	std::vector<Widget*> m_widgets;
};

template <typename T, typename... Args>
void EditorUI::attach(Args&&... args)
{ 
	m_widgets.push_back(new T(std::forward<Args>(args)...)); 
}

}