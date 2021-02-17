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
#include "../Component/Animator.h"
#include "../Component/Coin.h"
#include "../Component/SoundInstance.h"

#include "../System/SoundSystem.h"

#include "Modal.h"

namespace aka {

template <typename T>
struct ComponentNode {
	static const char* icon() { return ""; }
	//static const char* name() { return "None"; }
	static bool draw(T& component) { Logger::error("Trying to draw an undefined component"); }
};

// Avoid conflict of ID uniqueness
struct UniqueID {
	UniqueID(const void* data) : data(data) {}

	const char* operator()(const char* label) {
		static char buffer[256];
		int err = snprintf(buffer, 256, "%s##%p", label, data);
		return buffer;
	}
	const void* data;
};

const char* ComponentNode<Transform2D>::icon() { return ICON_FA_ARROWS_ALT; }
bool ComponentNode<Transform2D>::draw(Transform2D& transform)
{
	ImGui::InputFloat2("Position", transform.position.data);
	ImGui::InputFloat2("Size", transform.size.data);
	ImGui::SliderAngle("Rotation", &transform.rotation());
	return false;
}

const char* ComponentNode<Animator>::icon() { return ICON_FA_PLAY; }
bool ComponentNode<Animator>::draw(Animator& animator)
{
	UniqueID u(&animator);
	char buffer[256];
	// Set the name of the current sprite
	char currentSpriteName[256] = "None";
	Sprite* currentSprite = animator.sprite;
	for (auto& sprite : SpriteManager::iterator)
		if (currentSprite == &sprite.second)
			STR_CPY(currentSpriteName, 256, sprite.first.c_str());
	snprintf(buffer, 256, "%s", currentSpriteName);
	if (ImGui::BeginCombo("Sprite", buffer))
	{
		for (auto& sprite : SpriteManager::iterator)
		{
			bool sameSprite = (currentSprite == &sprite.second);
			snprintf(buffer, 256, "%s", sprite.first.c_str());
			if (ImGui::Selectable(buffer, sameSprite))
			{
				if (!sameSprite)
				{
					animator.sprite = &sprite.second;
					animator.currentAnimation = 0;
					animator.currentFrame = 0;
					animator.update();
					STR_CPY(currentSpriteName, 256, sprite.first.c_str());
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
		snprintf(buffer, 256, "%s (%" PRIu64 " ms)", anim.name.c_str(), anim.duration().milliseconds());
		bool currentAnimation = animator.currentAnimation == i;
		if (ImGui::RadioButton(buffer, currentAnimation))
		{
			animator.currentAnimation = i;
			animator.currentFrame = 0;
			animator.update();
		}
		i++;
	}
	ImGui::SliderInt(u("Frame"), reinterpret_cast<int*>(&animator.currentFrame), 0, (int)animator.sprite->animations[animator.currentAnimation].frames.size() - 1);
	ImGui::SliderInt(u("Layer"), &animator.layer, -20, 20);
	return false;
}

const char* ComponentNode<Collider2D>::icon() { return ICON_FA_SQUARE; }
bool ComponentNode<Collider2D>::draw(Collider2D& collider)
{
	UniqueID u(&collider);
	ImGui::InputFloat2(u("Position"), collider.position.data);
	ImGui::InputFloat2(u("Size"), collider.size.data);
	ImGui::InputFloat(u("Bouncing"), &collider.bouncing, 0.1f, 1.f);
	ImGui::InputFloat(u("Friction"), &collider.friction, 0.1f, 1.f);
	return false;
}

const char* ComponentNode<RigidBody2D>::icon() { return ICON_FA_APPLE_ALT; }
bool ComponentNode<RigidBody2D>::draw(RigidBody2D& rigid)
{
	UniqueID u(&rigid);
	ImGui::InputFloat(u("Mass"), &rigid.mass, 0.1f, 1.f);
	ImGui::InputFloat2(u("Acceleration"), rigid.acceleration.data);
	ImGui::InputFloat2(u("Velocity"), rigid.velocity.data);
	return false;
}

const char* ComponentNode<Camera2D>::icon() { return ICON_FA_CAMERA; }
bool ComponentNode<Camera2D>::draw(Camera2D& camera)
{
	ImGui::InputFloat2("Viewport", camera.camera.viewport.data);
	ImGui::Checkbox("Clamp", &camera.clampBorder);
	ImGui::Checkbox("Main", &camera.main);
	return false;
}

const char* ComponentNode<SoundInstance>::icon() { return ICON_FA_MUSIC; }
bool ComponentNode<SoundInstance>::draw(SoundInstance& audio)
{
	bool needUpdate = false;
	UniqueID u(&audio);
	char currentAudio[256] = "None";
	for (auto& a : AudioManager::iterator)
		if (audio.audio == a.second)
			STR_CPY(currentAudio, 256, a.first.c_str());
	if (ImGui::BeginCombo("Audio", currentAudio))
	{
		for (auto& it : AudioManager::iterator)
		{
			bool same = (audio.audio == it.second);
			if (ImGui::Selectable(it.first.c_str(), same))
			{
				if (!same)
				{
					AudioBackend::close(audio.audio);
					audio.audio = it.second;
					AudioBackend::play(audio.audio, audio.volume, audio.loop);
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
	ASSERT(error > 0, "");
	ImGui::ProgressBar(current / duration, ImVec2(0.f, 0.f), buffer);
	if (ImGui::Button(ICON_FA_BACKWARD))
		audio.audio->seek((uint64_t)max(current - 10.f, 0.f) * audio.audio->frequency());
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAUSE))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FORWARD))
		audio.audio->seek((uint64_t)min(current + 10.f, duration) * audio.audio->frequency());
	
	if (ImGui::SliderFloat(u("Volume"), &audio.volume, 0.f, 2.f))
	{
		needUpdate = true;
	}
	if (ImGui::Checkbox(u("Loop"), &audio.loop))
	{
		needUpdate = true;
	}
	return needUpdate;
}

const char* ComponentNode<Coin>::icon() { return ICON_FA_COINS; }
bool ComponentNode<Coin>::draw(Coin& coin)
{
	UniqueID u(&coin);
	ImGui::Checkbox(u("Picked"), &coin.picked);
	return false;
}

const char* ComponentNode<Text>::icon() { return ICON_FA_FONT; }
bool ComponentNode<Text>::draw(Text& text)
{
	UniqueID u(&text);
	ImGui::ColorEdit4(u("Color"), text.color.data);
	ImGui::SliderInt(u("Layer"), &text.layer, -20, 20);

	uint32_t currentHeight = text.font->height();
	const char* currentFont = text.font->family().c_str();
	static char buffer[256];
	snprintf(buffer, 256, "%s (%u)", text.font->family().c_str(), text.font->height());
	if (ImGui::BeginCombo("Font", buffer))
	{
		for (auto& font : FontManager::iterator)
		{
			bool sameHeight = (currentHeight == font.second.height());
			bool sameFamily = (currentFont == font.second.family().c_str());
			bool sameFont = sameHeight && sameFamily;
			snprintf(buffer, 256, "%s (%u)", font.second.family().c_str(), font.second.height());
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
	STR_CPY(t, 256, text.text.c_str());
	if (ImGui::InputTextWithHint(u("Text"), "Text to display", t, 256))
		text.text = t;
	return false;
}

void keySelector(const char *label, input::Key& currentKey)
{
	std::string currentName = input::getKeyName(currentKey);
	if (ImGui::BeginCombo(label, currentName.c_str()))
	{
		for (uint32_t iKey = 0; iKey < (uint32_t)input::Key::Count; iKey++)
		{
			input::Key key = (input::Key)iKey;
			bool sameKey = key == currentKey;
			std::string name = input::getKeyName(key);
			if (ImGui::Selectable(name.c_str(), sameKey))
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

const char* ComponentNode<Player>::icon() { return ICON_FA_RUNNING; }
bool ComponentNode<Player>::draw(Player& player)
{
	UniqueID u(&player);
	ImGui::SliderInt(u("Coin"), &player.coin, 0, 50);
	float metric = player.speed.metric();
	if (ImGui::SliderFloat(u("Speed"), &metric, 0.f, 50.f))
		player.speed = Speed(metric);
	keySelector("Jump", player.jump);
	keySelector("Left", player.left);
	keySelector("Right", player.right);
	player.state;
	return false;
}

const char* ComponentNode<TileLayer>::icon() { return ICON_FA_LAYER_GROUP; }
bool ComponentNode<TileLayer>::draw(TileLayer& layer)
{
	UniqueID u(&layer);
	vec2i gridCount = vec2i(layer.gridCount);
	vec2i gridSize = vec2i(layer.gridSize);
	if (ImGui::InputInt2(u("Grid count"), gridCount.data))
		layer.gridCount = vec2u(gridCount);
	if (ImGui::InputInt2(u("Grid size"), gridSize.data))
		layer.gridSize = vec2u(gridSize);

	static int id = 0;
	if (layer.tileID.size() >= 4)
	{
		ImGui::SliderInt(u("Index"), &id, 0, (int)layer.tileID.size() - 4);
		ImGui::InputInt4(u("TileID"), layer.tileID.data() + id);
	}
	ImGui::SliderInt(u("Layer"), &layer.layer, -20, 20);
	ImGui::ColorEdit4(u("Color"), layer.color.data, ImGuiColorEditFlags_Float);
	return false;
}

const char* ComponentNode<TileMap>::icon() { return ICON_FA_ATLAS; }
bool ComponentNode<TileMap>::draw(TileMap& map)
{
	UniqueID u(&map);
	vec2i gridCount = vec2i(map.gridCount);
	vec2i gridSize = vec2i(map.gridSize);
	if (ImGui::InputInt2(u("Grid count"), gridCount.data))
		map.gridCount = vec2u(gridCount);
	if (ImGui::InputInt2(u("Grid size"), gridSize.data))
		map.gridSize = vec2u(gridSize);
	if (map.texture == nullptr)
	{
		Path path;
		if (Modal::LoadButton(u("Load image"), &path))
		{
			try
			{
				Image image = Image::load(path);
				Sampler sampler;
				sampler.filterMag = aka::Sampler::Filter::Nearest;
				sampler.filterMin = aka::Sampler::Filter::Nearest;
				sampler.wrapS = aka::Sampler::Wrap::Clamp;
				sampler.wrapT = aka::Sampler::Wrap::Clamp;
				map.texture = Texture::create(image.width, image.height, Texture::Format::UnsignedByte, Texture::Component::RGBA, sampler);
				map.texture->upload(image.bytes.data());
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

template <typename T>
void component(World &world, Entity entity)
{
	static char buffer[256];
	if (entity.has<T>())
	{
		T& component = entity.get<T>();
		snprintf(buffer, 256, "%s %s##%p", ComponentNode<T>::icon(), ComponentHandle<T>::name, &component);
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
	if (ComponentType::get<Transform2D>() == filterComponentID && entity.has<Transform2D>())
		return true;
	if (ComponentType::get<Animator>() == filterComponentID && entity.has<Animator>())
		return true;
	if (ComponentType::get<Collider2D>() == filterComponentID && entity.has<Collider2D>())
		return true;
	if (ComponentType::get<RigidBody2D>() == filterComponentID && entity.has<RigidBody2D>())
		return true;
	if (ComponentType::get<Text>() == filterComponentID && entity.has<Text>())
		return true;
	if (ComponentType::get<TileMap>() == filterComponentID && entity.has<TileMap>())
		return true;
	if (ComponentType::get<TileLayer>() == filterComponentID && entity.has<TileLayer>())
		return true;
	if (ComponentType::get<Coin>() == filterComponentID && entity.has<Coin>())
		return true;
	if (ComponentType::get<Player>() == filterComponentID && entity.has<Player>())
		return true;
	if (ComponentType::get<Camera2D>() == filterComponentID && entity.has<Camera2D>())
		return true;
	if (ComponentType::get<SoundInstance>() == filterComponentID && entity.has<SoundInstance>())
		return true;
	return false;
}

// Draw an overlay over current widget components
void overlay(World &world, Entity entity)
{
	Entity cameraEntity = Entity::null();
	world.each([&cameraEntity](Entity entity) {
		if (entity.has<Camera2D>())
			cameraEntity = entity;
	});
	if (cameraEntity.valid())
	{
		Transform2D& cameraTransform = cameraEntity.get<Transform2D>();
		Camera2D& camera = cameraEntity.get<Camera2D>();
		mat3f view = mat3f::inverse(cameraTransform.model());
		Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
		vec2f scale = vec2f((float)backbuffer->width(), (float)backbuffer->height()) / camera.camera.viewport;
		ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(236.f / 255.f, 11.f / 255.f, 67.f / 255.f, 1.f));
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		// Animator overlay for current entity
		if (entity.has<Transform2D>() && entity.has<Animator>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Animator& a = entity.get<Animator>();
			const Sprite::Frame& f = a.getCurrentSpriteFrame();
			vec2f p = vec2f(view * t.model() * vec3f(0, 0, 1));
			p.y = camera.camera.viewport.y - p.y;
			vec2f s = vec2f(view * t.model() * vec3f((float)f.width, (float)f.height, 0));
			ImVec2 pos0 = ImVec2(scale.x * p.x, scale.y * p.y);
			ImVec2 pos1 = ImVec2(scale.x * (p.x + s.x), scale.y * (p.y - s.y));
			drawList->AddRect(pos0, pos1, color, 0.f, ImDrawCornerFlags_All, 2.f);
		}
		// Collider overlay for current entity
		else if (entity.has<Transform2D>() && entity.has<Collider2D>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Collider2D& c = entity.get<Collider2D>();
			vec2f p = vec2f(view * t.model() * vec3f(c.position, 1));
			p.y = camera.camera.viewport.y - p.y;
			vec2f s = vec2f(view * t.model() * vec3f(c.size, 0));
			ImVec2 pos0 = ImVec2(scale.x * p.x, scale.y * p.y);
			ImVec2 pos1 = ImVec2(scale.x * (p.x + s.x), scale.y * (p.y - s.y));
			drawList->AddRect(pos0, pos1, color, 0.f, ImDrawCornerFlags_All, 2.f);
		}
		// Text overlay for current entity
		else if (entity.has<Text>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Text& text = entity.get<Text>();
			vec2i size = text.font->size(text.text);
			vec2f p = vec2f(view * t.model() * vec3f(text.offset, 1));
			p.y = camera.camera.viewport.y - p.y;
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
		if (entity.has<Camera2D>())
			cameraEntity = entity;
	});
	if (!cameraEntity.valid())
		return Entity::null();
	Transform2D& cameraTransform = cameraEntity.get<Transform2D>();
	Camera2D& camera = cameraEntity.get<Camera2D>();
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	const vec2f scale = vec2f((float)backbuffer->width(), (float)backbuffer->height()) / camera.camera.viewport;
	const mat3f cam = cameraTransform.model();
	const mat3f view = mat3f::inverse(cam);
	const vec2f screenPos(input::mouse().x, backbuffer->height() - input::mouse().y);
	bool found = false;
	int32_t layer = -100;
	Entity picked = Entity::null();
	world.each([&](Entity entity) {
		if (!entity.has<Transform2D>() || found)
			return;
		if (entity.has<Collider2D>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Collider2D& c = entity.get<Collider2D>();
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
		else if (entity.has<Animator>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Animator& a = entity.get<Animator>();
			const Sprite::Frame& f = a.getCurrentSpriteFrame();
			// local to world to view space (320x180)
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
		else if (entity.has<Text>())
		{
			Transform2D& t = entity.get<Transform2D>();
			Text& text = entity.get<Text>();
			vec2i size = text.font->size(text.text);
			// local to world to view space (320x180)
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
					if (ImGui::MenuItem(ComponentHandle<Transform2D>::name, nullptr, nullptr, !m_currentEntity.has<Transform2D>()))
						m_currentEntity.add<Transform2D>(Transform2D());
					if (ImGui::BeginMenu(ComponentHandle<Animator>::name, !m_currentEntity.has<Animator>()))
					{
						for (auto &it : SpriteManager::iterator)
							if (ImGui::MenuItem(it.first.c_str(), nullptr, nullptr, true))
								m_currentEntity.add<Animator>(Animator(&it.second, 0));
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem(ComponentHandle<Collider2D>::name, nullptr, nullptr, !m_currentEntity.has<Collider2D>()))
						m_currentEntity.add<Collider2D>(Collider2D());
					if (ImGui::MenuItem(ComponentHandle<RigidBody2D>::name, nullptr, nullptr, !m_currentEntity.has<RigidBody2D>()))
						m_currentEntity.add<RigidBody2D>(RigidBody2D());
					if (ImGui::BeginMenu(ComponentHandle<Text>::name, !m_currentEntity.has<Text>()))
					{
						for (auto &it : FontManager::iterator)
							if (ImGui::MenuItem(it.first.c_str(), nullptr, nullptr, true))
								m_currentEntity.add<Text>(Text(vec2f(0.f), &it.second, "", color4f(1.f), 0));
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem(ComponentHandle<TileMap>::name, nullptr, nullptr, !m_currentEntity.has<TileMap>()))
						m_currentEntity.add<TileMap>(TileMap());
					if (ImGui::MenuItem(ComponentHandle<TileLayer>::name, nullptr, nullptr, !m_currentEntity.has<TileLayer>()))
						m_currentEntity.add<TileLayer>(TileLayer());
					if (ImGui::MenuItem(ComponentHandle<Coin>::name, nullptr, nullptr, !m_currentEntity.has<Coin>()))
						m_currentEntity.add<Coin>(Coin());
					if (ImGui::MenuItem(ComponentHandle<Player>::name, nullptr, nullptr, !m_currentEntity.has<Player>()))
						m_currentEntity.add<Player>(Player());
					if (ImGui::MenuItem(ComponentHandle<Camera2D>::name, nullptr, nullptr, !m_currentEntity.has<Camera2D>()))
						m_currentEntity.add<Camera2D>(Camera2D());
					if (ImGui::BeginMenu(ComponentHandle<SoundInstance>::name, !m_currentEntity.has<SoundInstance>()))
					{
						for (auto &it : AudioManager::iterator)
							if (ImGui::MenuItem(it.first.c_str(), nullptr, nullptr, true))
								m_currentEntity.add<SoundInstance>(SoundInstance(it.second, 1.f, false));
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove", m_currentEntity.valid())) {
					if (ImGui::MenuItem(ComponentHandle<Transform2D>::name, nullptr, nullptr, m_currentEntity.has<Transform2D>()))
						m_currentEntity.remove<Transform2D>();
					if (ImGui::MenuItem(ComponentHandle<Animator>::name, nullptr, nullptr, m_currentEntity.has<Animator>()))
						m_currentEntity.remove<Animator>();
					if (ImGui::MenuItem(ComponentHandle<Collider2D>::name, nullptr, nullptr, m_currentEntity.has<Collider2D>()))
						m_currentEntity.remove<Collider2D>();
					if (ImGui::MenuItem(ComponentHandle<RigidBody2D>::name, nullptr, nullptr, m_currentEntity.has<RigidBody2D>()))
						m_currentEntity.remove<RigidBody2D>();
					if (ImGui::MenuItem(ComponentHandle<Text>::name, nullptr, nullptr, m_currentEntity.has<Text>()))
						m_currentEntity.remove<Text>();
					if (ImGui::MenuItem(ComponentHandle<TileMap>::name, nullptr, nullptr, m_currentEntity.has<TileMap>()))
						m_currentEntity.remove<TileMap>();
					if (ImGui::MenuItem(ComponentHandle<TileLayer>::name, nullptr, nullptr, m_currentEntity.has<TileLayer>()))
						m_currentEntity.remove<TileLayer>();
					if (ImGui::MenuItem(ComponentHandle<Coin>::name, nullptr, nullptr, m_currentEntity.has<Coin>()))
						m_currentEntity.remove<Coin>();
					if (ImGui::MenuItem(ComponentHandle<Player>::name, nullptr, nullptr, m_currentEntity.has<Player>()))
						m_currentEntity.remove<Player>();
					if (ImGui::MenuItem(ComponentHandle<Camera2D>::name, nullptr, nullptr, m_currentEntity.has<Camera2D>()))
						m_currentEntity.remove<Camera2D>();
					if (ImGui::MenuItem(ComponentHandle<SoundInstance>::name, nullptr, nullptr, m_currentEntity.has<SoundInstance>()))
						m_currentEntity.remove<SoundInstance>();
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
		items[ComponentType::get<Transform2D>()] = ComponentHandle<Transform2D>::name;
		items[ComponentType::get<Animator>()] = ComponentHandle<Animator>::name;
		items[ComponentType::get<Player>()] = ComponentHandle<Player>::name;
		items[ComponentType::get<Collider2D>()] = ComponentHandle<Collider2D>::name;
		items[ComponentType::get<RigidBody2D>()] = ComponentHandle<RigidBody2D>::name;
		items[ComponentType::get<Camera2D>()] = ComponentHandle<Camera2D>::name;
		items[ComponentType::get<SoundInstance>()] = ComponentHandle<SoundInstance>::name;
		items[ComponentType::get<Coin>()] = ComponentHandle<Coin>::name;
		items[ComponentType::get<Text>()] = ComponentHandle<Text>::name;
		items[ComponentType::get<TileLayer>()] = ComponentHandle<TileLayer>::name;
		items[ComponentType::get<TileMap>()] = ComponentHandle<TileMap>::name;

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
				UniqueID u((void*)entity.handle());
				index++;
				if (filterEntities && !filterValid(entity, componentID))
					return;
				char buffer[256];
				snprintf(buffer, 256, "Entity %02u", index);
				bool selected = m_currentEntity == entity;
				// ---
				if (entity.valid())
				{
					if (entity.has<Transform2D>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Transform2D>::icon());
					if (entity.has<Camera2D>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Camera2D>::icon());
					if (entity.has<SoundInstance>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<SoundInstance>::icon());
					if (entity.has<Animator>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Animator>::icon());
					if (entity.has<Collider2D>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Collider2D>::icon());
					if (entity.has<RigidBody2D>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<RigidBody2D>::icon());
					if (entity.has<Text>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Text>::icon());
					if (entity.has<TileMap>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<TileMap>::icon());
					if (entity.has<TileLayer>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<TileLayer>::icon());
					if (entity.has<Coin>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Coin>::icon());
					if (entity.has<Player>())
						snprintf(buffer, 256, "%s %s", buffer, ComponentNode<Player>::icon());
				}
				else
					snprintf(buffer, 256, "%s %s", buffer, ICON_FA_TIMES);
				// ---
				if (ImGui::Selectable(buffer, &selected))
					m_currentEntity = entity;
				if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
					ImGui::OpenPopup(u("ClosePopup"));
				if (ImGui::BeginPopupContextItem(u("ClosePopup")))
				{
					if (ImGui::MenuItem(u("Delete entity")))
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
				component<Transform2D>(world, m_currentEntity);
				component<SoundInstance>(world, m_currentEntity);
				component<Collider2D>(world, m_currentEntity);
				component<RigidBody2D>(world, m_currentEntity);
				component<Camera2D>(world, m_currentEntity);
				component<Animator>(world, m_currentEntity);
				component<TileLayer>(world, m_currentEntity);
				component<TileMap>(world, m_currentEntity);
				component<Text>(world, m_currentEntity);
				component<Player>(world, m_currentEntity);
				component<Coin>(world, m_currentEntity);
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