#include <Aka/Scene/System.h>

namespace aka {

class ParticleSystem : public System
{
public:
	void onFixedUpdate(World& world, Time::Unit deltaTime);
	void onRender(World& world);
};

};