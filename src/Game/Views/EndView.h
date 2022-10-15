#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"

namespace aka {

class EndView : public View {
public:
	void onUpdate(Time dt) override;
	void onRender(gfx::Frame* frame) override;
};

};