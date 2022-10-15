#pragma once
#include <Aka/Aka.h>

#include <imgui.h>

#include <Aka/Resource/Resource.h>
#include <Aka/Resource/Asset.h>
#include <Aka/Resource/Shader/ShaderRegistry.h>
#include <Aka/Resource/Shader/Shader.h>

namespace aka {

struct EditorWidget {
	virtual ~EditorWidget() {}
	virtual void initialize() {}
	virtual void destroy() {}
	virtual void update(World& world) {}
	virtual void draw(World& world) {}
};

struct PauseGameEvent {
	bool pause = false;
};

class EditorApp :
	public Application,
	EventListener<ViewChangedEvent>,
	EventListener<PauseGameEvent>
{
public:
	EditorApp();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFrame() override;
	void onFixedUpdate(Time deltaTime) override;
	void onUpdate(Time deltaTime) override;
	void onRender(gfx::Frame* frame) override;
	void onResize(uint32_t width, uint32_t height) override;
	void onPresent() override;
	void onReceive(const ViewChangedEvent& event) override;
	void onReceive(const PauseGameEvent& event) override;
private:
	Time m_shaderReloadTimeElapsed;
	bool m_paused = false;
	std::vector<EditorWidget*> m_widgets;
	View::Ptr m_view;
	World* m_world;
};

};