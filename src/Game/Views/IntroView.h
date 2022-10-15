#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"

namespace aka {

class IntroView : public View {
public:
	void onCreate() override;
	void onDestroy() override;
	void onUpdate(Time dt) override;
	void onRender(gfx::Frame* frame) override;
private:
	float m_logoAlpha;
	Time m_elapsed;
};

};