#include "InfoWidget.h"

#include "../Component/Collider2D.h"
#include "../Component/Player.h"
#include "../Component/Animator.h"
#include "../Component/Coin.h"

namespace aka {

void InfoWidget::draw(World& world)
{
	uint32_t width, height;
	PlatformBackend::getSize(&width, &height);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowPos(ImVec2((float)(width - 5), 25.f), ImGuiCond_Always, ImVec2(1.f, 0.f));
	if (ImGui::Begin("Info", nullptr, flags))
	{
		static Device device = Device::getDefault();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Resolution : %ux%u", width, height);
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		//ImGui::Text("Draw call : %zu", m_batch.batchCount());
		//ImGui::Text("Vertices : %zu", m_batch.verticesCount());
		//ImGui::Text("Indices : %zu", m_batch.indicesCount());
		ImGui::Separator();
		const char* apiName[] = {
			"OpenGL",
			"DirectX11"
		};
		ImGui::Text("Api : %s", apiName[(int)GraphicBackend::api()]);
		ImGui::Text("Device : %s", device.vendor);
		ImGui::Text("Renderer : %s", device.renderer);
	}
	ImGui::End();

	/*static bool renderColliders = false;
	if (ImGui::Checkbox("Render colliders", &renderColliders))
	{
		auto view = world.registry().view<Collider2D>();
		for (entt::entity e : view)
		{
			Entity entity(e, &world);
			if (renderColliders)
			{
				if (!entity.has<Animator>())
					entity.add<Animator>(Animator(&SpriteManager::get("Collider"), 2));
			}
			else
			{
				if (entity.has<Animator>() && !entity.has<Player>() && !entity.has<Coin>())
					entity.remove<Animator>();
			}
		}
	}*/
}

};