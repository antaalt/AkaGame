#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"
#include "../Level.h"
#include "../Game.h"

namespace aka {

class IntroView : public View {
public:
	void onCreate() override;
	void onDestroy() override;
	void onUpdate(Router& router, Time::Unit dt) override;
	void onRender() override;
private:
	bool m_redraw;
	float m_logoAlpha;
	Batch m_batch;
	Time::Unit m_elapsed;
};

};