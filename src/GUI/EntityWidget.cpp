#include "EntityWidget.h"

#include <Aka/Aka.h>

#include <inttypes.h>
#include <map>

#include <IconsFontAwesome5.h>

#include "../Component/Transform2D.h"
#include "../Component/Collider2D.h"
#include "../Component/Camera2D.h"
#include "../Component/Text.h"
#include "../Component/TileMap.h"
#include "../Component/TileLayer.h"
#include "../Component/Player.h"
#include "../Component/SpriteAnimator.h"
#include "../Component/Coin.h"
#include "../Component/SoundInstance.h"
#include "../Component/Hurtable.h"
#include "../Component/Particle.h"

#include "../System/SoundSystem.h"

#include "../Game/Resources.h"

#include "Modal.h"

namespace aka {

template <typename T>
struct ComponentNode {
	static const char* name() { return "Unknown"; }
	static const char* icon() { return ""; }
	//static const char* name() { return "None"; }
	static bool draw(T& component) { Logger::error("Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<Transform2DComponent>::name() { return "Transform2DComponent"; }
template <> const char* ComponentNode<Transform2DComponent>::icon() { return ICON_FA_ARROWS_ALT; }
template <> bool ComponentNode<Transform2DComponent>::draw(Transform2DComponent& transform)
{
	ImGui::InputFloat2("Position", transform.position.data);
	ImGui::InputFloat2("Size", transform.size.data);
	ImGui::InputFloat2("Pivot", transform.pivot.data);
	float rot = transform.rotation.radian();
	if (ImGui::SliderAngle("Rotation", &rot))
		transform.rotation = anglef::radian(rot);
	return false;
}

template <> const char* ComponentNode<SpriteAnimatorComponent>::name() { return "SpriteAnimator"; }
template <> const char* ComponentNode<SpriteAnimatorComponent>::icon() { return ICON_FA_PLAY; }
template <> bool ComponentNode<SpriteAnimatorComponent>::draw(SpriteAnimatorComponent& animator)
{
	char buffer[256];
	// Set the name of the current sprite
	char currentSpriteName[256] = "None";
	Sprite* currentSprite = animator.sprite;
	for (auto& sprite : SpriteManager::iterator)
		if (currentSprite == &sprite.second)
			String::copy(currentSpriteName, 256, sprite.first.cstr());
	snprintf(buffer, 256, "%s", currentSpriteName);
	if (ImGui::BeginCombo("Sprite", buffer))
	{
		for (auto& sprite : SpriteManager::iterator)
		{
			bool sameSprite = (currentSprite == &sprite.second);
			snprintf(buffer, 256, "%s", sprite.first.cstr());
			if (ImGui::Selectable(buffer, sameSprite))
			{
				if (!sameSprite)
				{
					animator.sprite = &sprite.second;
					animator.currentAnimation = 0;
					animator.currentFrame = 0;
					animator.update();
					String::copy(currentSpriteName, 256, sprite.first.cstr());
				}
			}
			if (sameSprite)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	uint32_t i = 0;
	for (Sprite::Animation& anim : animator.sprite->animations)
	{
		snprintf(buffer, 256, "%s (%" PRIu64 " ms)", anim.name.cstr(), anim.duration().milliseconds());
		bool currentAnimation = animator.currentAnimation == i;
		if (ImGui::RadioButton(buffer, currentAnimation))
		{
			animator.currentAnimation = i;
			animator.currentFrame = 0;
			animator.update();
		}
		i++;
	}
	ImGui::SliderInt("Frame", reinterpret_cast<int*>(&animator.currentFrame), 0, (int)animator.sprite->animations[animator.currentAnimation].frames.size() - 1);
	ImGui::SliderInt("Layer", &animator.layer, -20, 20);
	return false;
}

template <> const char* ComponentNode<Collider2DComponent>::name() { return "Collider2D"; }
template <> const char* ComponentNode<Collider2DComponent>::icon() { return ICON_FA_SQUARE; }
template <> bool ComponentNode<Collider2DComponent>::draw(Collider2DComponent& collider)
{
	ImGui::InputFloat2("Position", collider.position.data);
	ImGui::InputFloat2("Size", collider.size.data);
	ImGui::InputFloat("Bouncing", &collider.bouncing, 0.1f, 1.f);
	ImGui::InputFloat("Friction", &collider.friction, 0.1f, 1.f);
	return false;
}

template <> const char* ComponentNode<RigidBody2DComponent>::name() { return "RigidBody2D"; }
template <> const char* ComponentNode<RigidBody2DComponent>::icon() { return ICON_FA_APPLE_ALT; }
template <> bool ComponentNode<RigidBody2DComponent>::draw(RigidBody2DComponent& rigid)
{
	ImGui::InputFloat("Mass", &rigid.mass, 0.1f, 1.f);
	ImGui::InputFloat2("Velocity", rigid.velocity.data);
	return false;
}

template <> const char* ComponentNode<Camera2DComponent>::name() { return "Camera2D"; }
template <> const char* ComponentNode<Camera2DComponent>::icon() { return ICON_FA_CAMERA; }
template <> bool ComponentNode<Camera2DComponent>::draw(Camera2DComponent& camera)
{
	ImGui::InputFloat("Left", &camera.camera.left);
	ImGui::InputFloat("Right", &camera.camera.right);
	ImGui::InputFloat("Top", &camera.camera.top);
	ImGui::InputFloat("Bottom", &camera.camera.bottom);
	ImGui::InputFloat("Near", &camera.camera.nearZ);
	ImGui::InputFloat("Far", &camera.camera.farZ);
	ImGui::Checkbox("Main", &camera.main);
	return false;
}

template <> const char* ComponentNode<SoundInstance>::name() { return "SoundInstance"; }
template <> const char* ComponentNode<SoundInstance>::icon() { return ICON_FA_MUSIC; }
template <> bool ComponentNode<SoundInstance>::draw(SoundInstance& audio)
{
	bool needUpdate = false;
	char currentAudio[256] = "None";
	for (auto& a : AudioManager::iterator)
		if (audio.audio == a.second)
			String::copy(currentAudio, 256, a.first.cstr());
	if (ImGui::BeginCombo("Audio", currentAudio))
	{
		for (auto& it : AudioManager::iterator)
		{
			bool same = (audio.audio == it.second);
			if (ImGui::Selectable(it.first.cstr(), same))
			{
				if (!same)
				{
					AudioBackend::close(audio.audio);
					audio.audio = it.second;
					AudioBackend::play(audio.audio);
					needUpdate = true;
				}
			}
			if (same)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	float current = audio.audio->offset() / (float)(audio.audio->frequency());
	float duration = audio.audio->samples() / (float)(audio.audio->frequency());
	ImGui::Text("Duration : %02u:%02u.%03u", (uint32_t)(duration / 60), (uint32_t)(duration) % 60, (uint32_t)(duration * 1000.f) % 1000);
	ImGui::Text("Frequency : %u", audio.audio->frequency());
	ImGui::Text("Channels : %u", audio.audio->channels());
	char buffer[10];
	int error = snprintf(buffer, 10, "%02u:%02u.%03u", (uint32_t)(current / 60), (uint32_t)(current) % 60, (uint32_t)(current * 1000.f) % 1000);
	AKA_ASSERT(error > 0, "");
	ImGui::ProgressBar(current / duration, ImVec2(0.f, 0.f), buffer);
	if (ImGui::Button(ICON_FA_BACKWARD))
		audio.audio->seek((uint64_t)max(current - 10.f, 0.f) * audio.audio->frequency());
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAUSE))
	{
		if (AudioBackend::playing(audio.audio))
			AudioBackend::close(audio.audio);
		else
			AudioBackend::play(audio.audio);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FORWARD))
		audio.audio->seek((uint64_t)min(current + 10.f, duration) * audio.audio->frequency());
	float volume = audio.audio->volume();
	if (ImGui::SliderFloat("Volume", &volume, 0.f, 2.f))
	{
		audio.audio->setVolume(volume);
		needUpdate = true;
	}
	if (ImGui::Checkbox("Loop", &audio.loop))
	{
		needUpdate = true;
	}
	return needUpdate;
}

template <> const char* ComponentNode<CoinComponent>::name() { return "Coin"; }
template <> const char* ComponentNode<CoinComponent>::icon() { return ICON_FA_COINS; }
template <> bool ComponentNode<CoinComponent>::draw(CoinComponent& coin)
{
	ImGui::Checkbox("Picked", &coin.picked);
	return false;
}

template <> const char* ComponentNode<Text2DComponent>::name() { return "Text2D"; }
template <> const char* ComponentNode<Text2DComponent>::icon() { return ICON_FA_FONT; }
template <> bool ComponentNode<Text2DComponent>::draw(Text2DComponent& text)
{
	ImGui::ColorEdit4("Color", text.color.data);
	ImGui::SliderInt("Layer", &text.layer, -20, 20);

	uint32_t currentHeight = text.font->height();
	const char* currentFont = text.font->family().cstr();
	static char buffer[256];
	snprintf(buffer, 256, "%s (%u)", text.font->family().cstr(), text.font->height());
	if (ImGui::BeginCombo("Font", buffer))
	{
		for (auto& font : FontManager::iterator)
		{
			bool sameHeight = (currentHeight == font.second.height());
			bool sameFamily = (currentFont == font.second.family().cstr());
			bool sameFont = sameHeight && sameFamily;
			snprintf(buffer, 256, "%s (%u)", font.second.family().cstr(), font.second.height());
			if (ImGui::Selectable(buffer, sameFont))
			{
				if (!sameFont)
					text.font = &font.second;
			}
			if (sameFont)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	char t[256];
	String::copy(t, 256, text.text.c_str());
	if (ImGui::InputTextWithHint("Text", "Text to display", t, 256))
		text.text = t;
	return false;
}

void keySelector(const char *label, KeyboardKey& currentKey)
{
	const char* currentName = Keyboard::name(currentKey);
	if (ImGui::BeginCombo(label, currentName))
	{
		for (uint32_t iKey = 0; iKey < (uint32_t)KeyboardKey::Count; iKey++)
		{
			KeyboardKey key = (KeyboardKey)iKey;
			bool sameKey = key == currentKey;
			const char* name = Keyboard::name(key);
			if (ImGui::Selectable(name, sameKey))
			{
				if (!sameKey)
					currentKey = key;
			}
			if (sameKey)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

template <> const char* ComponentNode<PlayerComponent>::name() { return "PlayerComponent"; }
template <> const char* ComponentNode<PlayerComponent>::icon() { return ICON_FA_RUNNING; }
template <> bool ComponentNode<PlayerComponent>::draw(PlayerComponent& player)
{
	ImGui::SliderInt("Coin", &player.coin, 0, 50);
	ImGui::SliderFloat("Speed", &player.speed, 0.f, 50.f);
	//keySelector("Jump", player.jump);
	//keySelector("Left", player.left);
	//keySelector("Right", player.right);
	player.state;
	return false;
}

template <> const char* ComponentNode<TileLayerComponent>::name() { return "TileLayer"; }
template <> const char* ComponentNode<TileLayerComponent>::icon() { return ICON_FA_LAYER_GROUP; }
template <> bool ComponentNode<TileLayerComponent>::draw(TileLayerComponent& layer)
{
	vec2i gridCount = vec2i(layer.gridCount);
	vec2i gridSize = vec2i(layer.gridSize);
	if (ImGui::InputInt2("Grid count", gridCount.data))
		layer.gridCount = vec2u(gridCount);
	if (ImGui::InputInt2("Grid size", gridSize.data))
		layer.gridSize = vec2u(gridSize);

	static int id = 0;
	if (layer.tileID.size() >= 4)
	{
		ImGui::SliderInt("Index", &id, 0, (int)layer.tileID.size() - 4);
		ImGui::InputInt4("TileID", layer.tileID.data() + id);
	}
	ImGui::SliderInt("Layer", &layer.layer, -20, 20);
	ImGui::ColorEdit4("Color", layer.color.data, ImGuiColorEditFlags_Float);
	return false;
}

template <> const char* ComponentNode<TileMapComponent>::name() { return "TileMap"; }
template <> const char* ComponentNode<TileMapComponent>::icon() { return ICON_FA_ATLAS; }
template <> bool ComponentNode<TileMapComponent>::draw(TileMapComponent& map)
{
	vec2i gridCount = vec2i(map.gridCount);
	vec2i gridSize = vec2i(map.gridSize);
	if (ImGui::InputInt2("Grid count", gridCount.data))
		map.gridCount = vec2u(gridCount);
	if (ImGui::InputInt2("Grid size", gridSize.data))
		map.gridSize = vec2u(gridSize);
	if (map.texture == nullptr)
	{
		Path path;
		if (Modal::LoadButton("Load image", &path))
		{
			try
			{
				Image image = Image::load(path);
				map.texture = Texture2D::create(image.width(), image.height(), TextureFormat::RGBA8, TextureFlag::ShaderResource);
				map.texture->upload(image.data());
			}
			catch (const std::exception&) {}
		}
	}
	else
	{
		float ratio = static_cast<float>(map.texture->width()) / static_cast<float>(map.texture->height());
		ImGui::Image((void*)map.texture->handle().value(), ImVec2(300, 300 * 1 / ratio), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1,1,1,1), ImVec4(0.5,0.5,0.5,1));
	}
	return false;
}

template <> const char* ComponentNode<Particle2DComponent>::name() { return "Particle2D"; }
template <> const char* ComponentNode<Particle2DComponent>::icon() { return ICON_FA_SMOG; }
template <> bool ComponentNode<Particle2DComponent>::draw(Particle2DComponent& particle)
{
	int birthTime = (int)particle.birthTime.milliseconds();
	if (ImGui::InputInt("Birth Time", &birthTime))
		particle.birthTime = Time::Unit::milliseconds(birthTime);
	int lifeTime = (int)particle.lifeTime.milliseconds();
	if (ImGui::InputInt("Life time", &lifeTime))
		particle.lifeTime = Time::Unit::milliseconds(lifeTime);
	ImGui::InputFloat2("Velocity", particle.velocity.data);

	float rot = particle.angularVelocity.radian();
	if (ImGui::SliderAngle("Rotation", &rot))
		particle.angularVelocity = anglef::radian(rot);
	ImGui::InputFloat2("Scale velocity", particle.scaleVelocity.data);
	ImGui::ColorEdit4("Color", particle.color.data, ImGuiColorEditFlags_Float);
	ImGui::SliderInt("Layer", &particle.layer, -20, 20);
	return false;
}

template <> const char* ComponentNode<HurtComponent>::name() { return "HurtComponent"; }
template <> const char* ComponentNode<HurtComponent>::icon() { return ICON_FA_HEART_BROKEN; }
template <> bool ComponentNode<HurtComponent>::draw(HurtComponent& hurt)
{
	return false;
}


template <typename T>
void component(World &world, Entity entity)
{
	static char buffer[256];
	if (entity.has<T>())
	{
		T& component = entity.get<T>();
		snprintf(buffer, 256, "%s %s##%p", ComponentNode<T>::icon(), ComponentNode<T>::name(), &component);
		if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			snprintf(buffer, 256, "ClosePopUp##%p", &component);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(buffer);
			if (ComponentNode<T>::draw(component))
			{
				world.registry().replace<T>(entity.handle(), component);
			}
			if (ImGui::BeginPopupContextItem(buffer))
			{
				if (ImGui::MenuItem("Remove"))
					entity.remove<T>();
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}
}

bool filterValid(Entity entity, ComponentID filterComponentID)
{
	if (ComponentType::get<Transform2DComponent>() == filterComponentID && entity.has<Transform2DComponent>())
		return true;
	if (ComponentType::get<SpriteAnimatorComponent>() == filterComponentID && entity.has<SpriteAnimatorComponent>())
		return true;
	if (ComponentType::get<Collider2DComponent>() == filterComponentID && entity.has<Collider2DComponent>())
		return true;
	if (ComponentType::get<RigidBody2DComponent>() == filterComponentID && entity.has<RigidBody2DComponent>())
		return true;
	if (ComponentType::get<Text2DComponent>() == filterComponentID && entity.has<Text2DComponent>())
		return true;
	if (ComponentType::get<TileMapComponent>() == filterComponentID && entity.has<TileMapComponent>())
		return true;
	if (ComponentType::get<TileLayerComponent>() == filterComponentID && entity.has<TileLayerComponent>())
		return true;
	if (ComponentType::get<CoinComponent>() == filterComponentID && entity.has<CoinComponent>())
		return true;
	if (ComponentType::get<PlayerComponent>() == filterComponentID && entity.has<PlayerComponent>())
		return true;
	if (ComponentType::get<Camera2DComponent>() == filterComponentID && entity.has<Camera2DComponent>())
		return true;
	if (ComponentType::get<SoundInstance>() == filterComponentID && entity.has<SoundInstance>())
		return true;
	if (ComponentType::get<Particle2DComponent>() == filterComponentID && entity.has<Particle2DComponent>())
		return true;
	if (ComponentType::get<HurtComponent>() == filterComponentID && entity.has<HurtComponent>())
		return true;
	return false;
}

// Draw an overlay over current widget components
void overlay(World &world, Entity entity)
{
	Entity cameraEntity = Entity::null();
	world.each([&cameraEntity](Entity entity) {
		if (entity.has<Camera2DComponent>())
			cameraEntity = entity;
	});
	if (cameraEntity.valid())
	{
		Transform2DComponent& cameraTransform = cameraEntity.get<Transform2DComponent>();
		Camera2DComponent& camera = cameraEntity.get<Camera2DComponent>();
		mat3f view = mat3f::inverse(cameraTransform.model());
		Framebuffer::Ptr backbuffer = GraphicBackend::device()->backbuffer();
		vec2f scale = vec2f((float)backbuffer->width(), (float)backbuffer->height()) / vec2f(camera.camera.right, camera.camera.top);
		ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(236.f / 255.f, 11.f / 255.f, 67.f / 255.f, 1.f));
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		// Animator overlay for current entity
		if (entity.has<Transform2DComponent>() && entity.has<SpriteAnimatorComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			SpriteAnimatorComponent& a = entity.get<SpriteAnimatorComponent>();
			const Sprite::Frame& f = a.getCurrentSpriteFrame();
			vec2f p = vec2f(view * t.model() * vec3f(0, 0, 1));
			p.y = camera.camera.top - p.y;
			vec2f s = vec2f(view * t.model() * vec3f((float)f.width, (float)f.height, 0));
			ImVec2 pos0 = ImVec2(scale.x * p.x, scale.y * p.y);
			ImVec2 pos1 = ImVec2(scale.x * (p.x + s.x), scale.y * (p.y - s.y));
			drawList->AddRect(pos0, pos1, color, 0.f, ImDrawCornerFlags_All, 2.f);
		}
		// Collider overlay for current entity
		else if (entity.has<Transform2DComponent>() && entity.has<Collider2DComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			Collider2DComponent& c = entity.get<Collider2DComponent>();
			vec2f p = vec2f(view * t.model() * vec3f(c.position, 1));
			p.y = camera.camera.top - p.y;
			vec2f s = vec2f(view * t.model() * vec3f(c.size, 0));
			ImVec2 pos0 = ImVec2(scale.x * p.x, scale.y * p.y);
			ImVec2 pos1 = ImVec2(scale.x * (p.x + s.x), scale.y * (p.y - s.y));
			drawList->AddRect(pos0, pos1, color, 0.f, ImDrawCornerFlags_All, 2.f);
		}
		// Text overlay for current entity
		else if (entity.has<Text2DComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			Text2DComponent& text = entity.get<Text2DComponent>();
			vec2i size = text.font->size(text.text);
			vec2f p = vec2f(view * t.model() * vec3f(text.offset, 1));
			p.y = camera.camera.top - p.y;
			vec2f s = vec2f(view * t.model() * vec3f(vec2f(size), 0));
			ImVec2 pos0 = ImVec2(scale.x * p.x, scale.y * p.y);
			ImVec2 pos1 = ImVec2(scale.x * (p.x + s.x), scale.y * (p.y - s.y));
			drawList->AddRect(pos0, pos1, color, 0.f, ImDrawCornerFlags_All, 2.f);
		}
	}
}

// Return the entity picked on click
Entity pickEntity(World &world)
{
	Entity cameraEntity = Entity::null();
	world.each([&cameraEntity](Entity entity) {
		if (entity.has<Camera2DComponent>())
			cameraEntity = entity;
	});
	if (!cameraEntity.valid())
		return Entity::null();
	Transform2DComponent& cameraTransform = cameraEntity.get<Transform2DComponent>();
	Camera2DComponent& camera = cameraEntity.get<Camera2DComponent>();
	Framebuffer::Ptr backbuffer = GraphicBackend::device()->backbuffer();
	const vec2f scale = vec2f((float)backbuffer->width(), (float)backbuffer->height()) / vec2f(camera.camera.right, camera.camera.top);
	const mat3f cam = cameraTransform.model();
	const mat3f view = mat3f::inverse(cam);
	const vec2f screenPos(Mouse::position().x, Mouse::position().y);
	bool found = false;
	int32_t layer = -100;
	Entity picked = Entity::null();
	world.each([&](Entity entity) {
		if (!entity.has<Transform2DComponent>() || found)
			return;
		if (entity.has<Collider2DComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			Collider2DComponent& c = entity.get<Collider2DComponent>();
			vec2f p = vec2f(view * t.model() * vec3f(c.position, 1));
			vec2f s = vec2f(view * t.model() * vec3f(c.size, 0));
			vec2f pos0 = scale * p;
			vec2f pos1 = scale * (p + s);
			if (pos0.x < screenPos.x && pos0.y < screenPos.y && pos1.x > screenPos.x && pos1.y > screenPos.y)
			{
				found = true;
				picked = entity;
			}
		}
		else if (entity.has<SpriteAnimatorComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			SpriteAnimatorComponent& a = entity.get<SpriteAnimatorComponent>();
			const Sprite::Frame& f = a.getCurrentSpriteFrame();
			// local to world to view space
			vec2f p = vec2f(view * t.model() * vec3f(0, 0, 1));
			vec2f s = vec2f(view * t.model() * vec3f((float)f.width, (float)f.height, 0));
			// scale to 1920x1080, bottom left
			vec2f pos0 = scale * p;
			vec2f pos1 = scale * (p + s);
			if (layer < a.layer && pos0.x <= screenPos.x && pos0.y <= screenPos.y && pos1.x >= screenPos.x && pos1.y >= screenPos.y)
			{
				layer = a.layer;
				picked = entity;
			}
		}
		else if (entity.has<Text2DComponent>())
		{
			Transform2DComponent& t = entity.get<Transform2DComponent>();
			Text2DComponent& text = entity.get<Text2DComponent>();
			vec2i size = text.font->size(text.text);
			// local to world to view space
			vec2f p = vec2f(view * t.model() * vec3f(text.offset, 1));
			vec2f s = vec2f(view * t.model() * vec3f(vec2f(size), 0));
			// scale to 1920x1080, bottom left
			vec2f pos0 = scale * p;
			vec2f pos1 = scale * (p + s);
			if (layer < text.layer && pos0.x <= screenPos.x && pos0.y <= screenPos.y && pos1.x >= screenPos.x && pos1.y >= screenPos.y)
			{
				layer = text.layer;
				picked = entity;
			}
		}
	});
	return picked;
}

void EntityWidget::draw(World& world)
{
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseReleased(0) && !io.WantCaptureMouse)
		m_currentEntity = pickEntity(world);

	if (ImGui::Begin("Entities##window", nullptr, ImGuiWindowFlags_MenuBar))
	{
		static ImVec4 color = ImVec4(236.f / 255.f, 11.f / 255.f, 67.f / 255.f, 1.f);

		if (ImGui::BeginMenuBar())
		{
			// Add component
			static int currentComponent = 0;
			if (ImGui::BeginMenu("Entity"))
			{
				if (ImGui::MenuItem("Create"))
				{
					//static char entityName[256];
					//ImGui::InputText("##entityName", entityName, 256);
					m_currentEntity = world.createEntity("");
				}
				if (ImGui::MenuItem("Destroy", nullptr, nullptr, m_currentEntity.valid()))
				{
					m_currentEntity.destroy();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component", m_currentEntity.valid()))
			{
				if (ImGui::BeginMenu("Add", m_currentEntity.valid())) {
					if (ImGui::MenuItem(ComponentNode<Transform2DComponent>::name(), nullptr, nullptr, !m_currentEntity.has<Transform2DComponent>()))
						m_currentEntity.add<Transform2DComponent>(Transform2DComponent());
					if (ImGui::BeginMenu(ComponentNode<SpriteAnimatorComponent>::name(), !m_currentEntity.has<SpriteAnimatorComponent>()))
					{
						for (auto &it : SpriteManager::iterator)
							if (ImGui::MenuItem(it.first.cstr(), nullptr, nullptr, true))
								m_currentEntity.add<SpriteAnimatorComponent>(SpriteAnimatorComponent(&it.second, 0));
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem(ComponentNode<Collider2DComponent>::name(), nullptr, nullptr, !m_currentEntity.has<Collider2DComponent>()))
						m_currentEntity.add<Collider2DComponent>(Collider2DComponent());
					if (ImGui::MenuItem(ComponentNode<RigidBody2DComponent>::name(), nullptr, nullptr, !m_currentEntity.has<RigidBody2DComponent>()))
						m_currentEntity.add<RigidBody2DComponent>(RigidBody2DComponent());
					if (ImGui::BeginMenu(ComponentNode<Text2DComponent>::name(), !m_currentEntity.has<Text2DComponent>()))
					{
						for (auto &it : FontManager::iterator)
							if (ImGui::MenuItem(it.first.cstr(), nullptr, nullptr, true))
								m_currentEntity.add<Text2DComponent>(Text2DComponent(vec2f(0.f), &it.second, "", color4f(1.f), 0));
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem(ComponentNode<TileMapComponent>::name(), nullptr, nullptr, !m_currentEntity.has<TileMapComponent>()))
						m_currentEntity.add<TileMapComponent>(TileMapComponent());
					if (ImGui::MenuItem(ComponentNode<TileLayerComponent>::name(), nullptr, nullptr, !m_currentEntity.has<TileLayerComponent>()))
						m_currentEntity.add<TileLayerComponent>(TileLayerComponent());
					if (ImGui::MenuItem(ComponentNode<CoinComponent>::name(), nullptr, nullptr, !m_currentEntity.has<CoinComponent>()))
						m_currentEntity.add<CoinComponent>(CoinComponent());
					if (ImGui::MenuItem(ComponentNode<PlayerComponent>::name(), nullptr, nullptr, !m_currentEntity.has<PlayerComponent>()))
						m_currentEntity.add<PlayerComponent>(PlayerComponent());
					if (ImGui::MenuItem(ComponentNode<Camera2DComponent>::name(), nullptr, nullptr, !m_currentEntity.has<Camera2DComponent>()))
						m_currentEntity.add<Camera2DComponent>(Camera2DComponent());
					if (ImGui::BeginMenu(ComponentNode<SoundInstance>::name(), !m_currentEntity.has<SoundInstance>()))
					{
						for (auto &it : AudioManager::iterator)
							if (ImGui::MenuItem(it.first.cstr(), nullptr, nullptr, true))
								m_currentEntity.add<SoundInstance>(SoundInstance(it.second, 1.f, false));
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem(ComponentNode<Particle2DComponent>::name(), nullptr, nullptr, !m_currentEntity.has<Particle2DComponent>()))
						m_currentEntity.add<Particle2DComponent>();
					if (ImGui::MenuItem(ComponentNode<HurtComponent>::name(), nullptr, nullptr, !m_currentEntity.has<HurtComponent>()))
						m_currentEntity.add<HurtComponent>();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove", m_currentEntity.valid())) {
					if (ImGui::MenuItem(ComponentNode<Transform2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<Transform2DComponent>()))
						m_currentEntity.remove<Transform2DComponent>();
					if (ImGui::MenuItem(ComponentNode<SpriteAnimatorComponent>::name(), nullptr, nullptr, m_currentEntity.has<SpriteAnimatorComponent>()))
						m_currentEntity.remove<SpriteAnimatorComponent>();
					if (ImGui::MenuItem(ComponentNode<Collider2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<Collider2DComponent>()))
						m_currentEntity.remove<Collider2DComponent>();
					if (ImGui::MenuItem(ComponentNode<RigidBody2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<RigidBody2DComponent>()))
						m_currentEntity.remove<RigidBody2DComponent>();
					if (ImGui::MenuItem(ComponentNode<Text2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<Text2DComponent>()))
						m_currentEntity.remove<Text2DComponent>();
					if (ImGui::MenuItem(ComponentNode<TileMapComponent>::name(), nullptr, nullptr, m_currentEntity.has<TileMapComponent>()))
						m_currentEntity.remove<TileMapComponent>();
					if (ImGui::MenuItem(ComponentNode<TileLayerComponent>::name(), nullptr, nullptr, m_currentEntity.has<TileLayerComponent>()))
						m_currentEntity.remove<TileLayerComponent>();
					if (ImGui::MenuItem(ComponentNode<CoinComponent>::name(), nullptr, nullptr, m_currentEntity.has<CoinComponent>()))
						m_currentEntity.remove<CoinComponent>();
					if (ImGui::MenuItem(ComponentNode<PlayerComponent>::name(), nullptr, nullptr, m_currentEntity.has<PlayerComponent>()))
						m_currentEntity.remove<PlayerComponent>();
					if (ImGui::MenuItem(ComponentNode<Camera2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<Camera2DComponent>()))
						m_currentEntity.remove<Camera2DComponent>();
					if (ImGui::MenuItem(ComponentNode<SoundInstance>::name(), nullptr, nullptr, m_currentEntity.has<SoundInstance>()))
						m_currentEntity.remove<SoundInstance>();
					if (ImGui::MenuItem(ComponentNode<Particle2DComponent>::name(), nullptr, nullptr, m_currentEntity.has<Particle2DComponent>()))
						m_currentEntity.remove<Particle2DComponent>();
					if (ImGui::MenuItem(ComponentNode<HurtComponent>::name(), nullptr, nullptr, m_currentEntity.has<HurtComponent>()))
						m_currentEntity.remove<HurtComponent>();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		static bool filterEntities = false;
		static const char noItem[256] = "All";
		static ComponentID componentID{};
		static std::map<ComponentID, const char*> items;
		items[ComponentType::get<Transform2DComponent>()] = ComponentNode<Transform2DComponent>::name();
		items[ComponentType::get<SpriteAnimatorComponent>()] = ComponentNode<SpriteAnimatorComponent>::name();
		items[ComponentType::get<PlayerComponent>()] = ComponentNode<PlayerComponent>::name();
		items[ComponentType::get<Collider2DComponent>()] = ComponentNode<Collider2DComponent>::name();
		items[ComponentType::get<RigidBody2DComponent>()] = ComponentNode<RigidBody2DComponent>::name();
		items[ComponentType::get<Camera2DComponent>()] = ComponentNode<Camera2DComponent>::name();
		items[ComponentType::get<SoundInstance>()] = ComponentNode<SoundInstance>::name();
		items[ComponentType::get<CoinComponent>()] = ComponentNode<CoinComponent>::name();
		items[ComponentType::get<Text2DComponent>()] = ComponentNode<Text2DComponent>::name();
		items[ComponentType::get<TileLayerComponent>()] = ComponentNode<TileLayerComponent>::name();
		items[ComponentType::get<TileMapComponent>()] = ComponentNode<TileMapComponent>::name();
		items[ComponentType::get<Particle2DComponent>()] = ComponentNode<Particle2DComponent>::name();
		items[ComponentType::get<HurtComponent>()] = ComponentNode<HurtComponent>::name();

		ImGui::Checkbox("##EnableFilter", &filterEntities);
		ImGui::SameLine();
		if (ImGui::BeginCombo("Filter", items[componentID]))
		{
			for (auto &it : items)
			{
				bool same = (componentID == it.first);
				if (ImGui::Selectable(it.second, same))
				{
					if (!same)
						componentID = it.first;
				}
				if (same)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginChild("##list", ImVec2(0, 200), true))
		{
			uint32_t index = 0;
			world.each([&](Entity entity) {
				index++;
				if (filterEntities && !filterValid(entity, componentID))
					return;
				char buffer[256];
				snprintf(buffer, 256, "Entity %02u", index);
				bool selected = m_currentEntity == entity;
				// ---
				if (entity.valid())
				{
					if (entity.has<Transform2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Transform2DComponent>::icon());
					if (entity.has<Camera2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Camera2DComponent>::icon());
					if (entity.has<SoundInstance>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<SoundInstance>::icon());
					if (entity.has<SpriteAnimatorComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<SpriteAnimatorComponent>::icon());
					if (entity.has<Collider2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Collider2DComponent>::icon());
					if (entity.has<RigidBody2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<RigidBody2DComponent>::icon());
					if (entity.has<Text2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Text2DComponent>::icon());
					if (entity.has<TileMapComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<TileMapComponent>::icon());
					if (entity.has<TileLayerComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<TileLayerComponent>::icon());
					if (entity.has<CoinComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<CoinComponent>::icon());
					if (entity.has<PlayerComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<PlayerComponent>::icon());
					if (entity.has<Particle2DComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Particle2DComponent>::icon());
					if (entity.has<HurtComponent>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<HurtComponent>::icon());
				}
				else
					snprintf(buffer, 256, "%s %s", buffer, ICON_FA_TIMES);
				// ---
				if (ImGui::Selectable(buffer, &selected))
					m_currentEntity = entity;
				if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
					ImGui::OpenPopup("ClosePopup");
				if (ImGui::BeginPopupContextItem("ClosePopup"))
				{
					if (ImGui::MenuItem("Delete entity"))
						m_currentEntity.destroy();
					ImGui::EndPopup();
				}
			});
		}
		ImGui::EndChild();
		// Add entity menu
		static char entityName[256];
		ImGui::InputTextWithHint("##entityName", "Entity name", entityName, 256);
		ImGui::SameLine();
		if (ImGui::Button("Add entity"))
		{
			filterEntities = false;
			m_currentEntity = world.createEntity(entityName);
		}
		ImGui::Separator();

		// --- Entity info
		ImGui::TextColored(color, "Entity");
		if (m_currentEntity.valid())
		{
			if (world.registry().orphan(m_currentEntity.handle()))
			{
				ImGui::Text("Add a component to the entity.");
			}
			else
			{
				component<Transform2DComponent>(world, m_currentEntity);
				component<SoundInstance>(world, m_currentEntity);
				component<Collider2DComponent>(world, m_currentEntity);
				component<RigidBody2DComponent>(world, m_currentEntity);
				component<Camera2DComponent>(world, m_currentEntity);
				component<SpriteAnimatorComponent>(world, m_currentEntity);
				component<TileLayerComponent>(world, m_currentEntity);
				component<TileMapComponent>(world, m_currentEntity);
				component<Text2DComponent>(world, m_currentEntity);
				component<PlayerComponent>(world, m_currentEntity);
				component<CoinComponent>(world, m_currentEntity);
				component<Particle2DComponent>(world, m_currentEntity);
				component<HurtComponent>(world, m_currentEntity);
				// Draw an overlay for current entity
				overlay(world, m_currentEntity);
			}
		}
		else
		{
			ImGui::Text("Select an entity to display its components.");
		}
	}
	ImGui::End();
}

};