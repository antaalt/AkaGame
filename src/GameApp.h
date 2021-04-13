#pragma once
#include <Aka/Aka.h>

namespace aka {

class GameApp : 
	public Application,
	EventListener<ViewChangedEvent>
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
private:
	View::Ptr m_view;
};

};