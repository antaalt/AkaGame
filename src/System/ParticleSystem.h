#include <Aka/Scene/System.h>

namespace aka {

class ParticleSystem : public System
{
public:
	void onFixedUpdate(World& world, Time deltaTime) override;
	void onRender(World& world, gfx::Frame* frame) override;
};

};