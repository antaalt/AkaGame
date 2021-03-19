#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Platform/Input.h>

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
	KeyboardKey jump = KeyboardKey::Space;
	KeyboardKey left = KeyboardKey::A;
	KeyboardKey right = KeyboardKey::D;
};

}