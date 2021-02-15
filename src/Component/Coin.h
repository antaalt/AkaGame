#pragma once

#include <Aka/Scene/Component.h>

namespace aka {

struct Coin
{
	bool picked = false;
};
template <>
const char* ComponentHandle<Coin>::name = "Coin";


}