#pragma once

#include <Aka/Scene/Component.h>

namespace aka {

struct Door {

};
template <>
const char* ComponentHandle<Door>::name = "Door";


}