#pragma once 

#include <Aka/Aka.h>

#include "../Resources.h"
#include "../Level.h"

namespace aka {

class EndView : public View {
public:
	void onCreate() override;
	void onDestroy() override;
	void onUpdate(Router& router, Time::Unit dt) override;
	void onRender() override;
private:
	Batch m_batch;
};

};