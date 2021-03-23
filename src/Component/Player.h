#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Core/Controller.h>

namespace aka {

struct PlayerComponent
{
	int coin = 0;

	enum class State {
		Idle,
		Walking,
		Jumping,
		DoubleJumping,
		Falling
	};
	State state = State::Idle;
	float speed = 6.f;
	ButtonController jump;
	MotionController motion;
};

}