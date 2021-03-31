#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"

namespace aka {

class IntroView : public View {
public:
	void onCreate() override;
	void onDestroy() override;
	void onUpdate(Time::Unit dt) override;
	void onRender() override;
private:
	float m_logoAlpha;
	Time::Unit m_elapsed;
};

};