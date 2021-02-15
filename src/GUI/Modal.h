#pragma once

#include <Aka/Aka.h>
#include "GUINode.h"

namespace aka {

namespace Modal {

bool LoadButton(const char* label, Path* path);

bool Error(const char* label, std::string& error);

};

};