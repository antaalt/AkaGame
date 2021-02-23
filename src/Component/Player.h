#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Platform/Input.h>

namespace aka {

struct Speed
{
	Speed(float metric) : m_speed(metric) {}

	// Generate speed in m/s
	static Speed metric(float metric) { return Speed(metric); }
	// Generate speed in pixel/s (16 pixels -> 1 m)
	static Speed pixel(float pixel) { return Speed(pixel / 16.f); }

	float metric() const { return m_speed; }
	float pixel() const { return m_speed * 16.f; }

private:
	float m_speed;
};

struct Player
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
	Speed speed = Speed(6.f);
	input::Key jump = input::Key::Space;
	input::Key left = input::Key::A;
	input::Key right = input::Key::D;
};
template <>
const char* ComponentHandle<Player>::name = "Player";

}