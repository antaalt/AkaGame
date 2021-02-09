#pragma once

#include <Aka/Core/ECS/Component.h>

namespace aka {

struct Coin : public Component {
	bool picked = false;
};


}