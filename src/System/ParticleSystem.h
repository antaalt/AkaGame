#include <Aka/Scene/System.h>

namespace aka {

class ParticleSystem : public System
{
public:
	void update(World& world, Time::Unit deltaTime);
	void draw(World& world);
};

};