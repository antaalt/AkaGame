#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"

namespace aka {

class EndView : public View {
public:
	void onUpdate(Time::Unit dt) override;
	void onRender() override;
};

};