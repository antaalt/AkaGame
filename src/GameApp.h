#pragma once
#include <Aka/Aka.h>

namespace aka {

class GameApp : 
	public Application,
	EventListener<ViewChangedEvent>
{
public:
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFrame() override;
	void onUpdate(Time deltaTime) override;
	void onRender(gfx::Frame* frame) override;
	void onResize(uint32_t width, uint32_t height) override;
	void onPresent() override;
	void onReceive(const ViewChangedEvent& event) override;
private:
	View::Ptr m_view;
};

};