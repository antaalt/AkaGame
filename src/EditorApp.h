#pragma once
#include <Aka/Aka.h>

#include <imgui.h>

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
	void onCreate() override;
	void onDestroy() override;
	void onFrame() override;
	void onUpdate(Time::Unit deltaTime) override;
	void onRender() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onPresent() override;
	void onReceive(const ViewChangedEvent& event) override;
	void onReceive(const PauseGameEvent& event) override;
private:
	bool m_paused = false;
	std::vector<EditorWidget*> m_widgets;
	View::Ptr m_view;
	World* m_world;
};

};