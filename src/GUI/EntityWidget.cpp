#include "EntityWidget.h"

#include <Aka/Aka.h>

#include <inttypes.h>

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

#include "../System/SoundSystem.h"

#include "Modal.h"

namespace aka {

template <typename T>
struct ComponentNode {
	static const char* icon();
	static const char* name();
	static void draw(T& component);
};

// Avoid conflict of ID uniqueness
struct UniqueID {
	UniqueID(void* data) : data(data) {}

	const char* operator()(const char* label) {
		static char buffer[256];
		int err = snprintf(buffer, 256, "%s##%p", label, data);
		return buffer;
	}
	void* data;
};

const char* ComponentNode<Transform2D>::name() { return "Transform2D"; }
const char* ComponentNode<Transform2D>::icon() { return ICON_FA_ARROWS_ALT; }
void ComponentNode<Transform2D>::draw(Transform2D& transform)
{
	UniqueID u(&transform);
	ImGui::InputFloat2(u("Position"), transform.model[2].data);
	vec2f size = vec2f(transform.model[0].x, transform.model[1].y);
	if (ImGui::InputFloat2(u("Size"), size.data))
	{
		transform.model[0].x = size.x;
		transform.model[1].y = size.y;
	}
	//ImGui::InputFloat("Rotation", &transform->rotation(), 0.1f, 1.f, 3);
}

const char* ComponentNode<Animator>::name() { return "Animator"; }
const char* ComponentNode<Animator>::icon() { return ICON_FA_PLAY; }
void ComponentNode<Animator>::draw(Animator& animator)
{
	UniqueID u(&animator);
	char buffer[256];
	// Set the name of the current sprite
	char currentSpriteName[256] = "None";
	Sprite* currentSprite = animator.sprite;
	for (auto& sprite : Resources::sprite)
		if (currentSprite == &sprite.second)
			STR_CPY(currentSpriteName, 256, sprite.first.c_str());
	snprintf(buffer, 256, "%s", currentSpriteName);
	if (ImGui::BeginCombo("Sprite", buffer))
	{
		for (auto& sprite : Resources::sprite)
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
}

const char* ComponentNode<Collider2D>::name() { return "Collider2D"; }
const char* ComponentNode<Collider2D>::icon() { return ICON_FA_SQUARE; }
void ComponentNode<Collider2D>::draw(Collider2D& collider)
{
	UniqueID u(&collider);
	ImGui::InputFloat2(u("Position"), collider.position.data);
	ImGui::InputFloat2(u("Size"), collider.size.data);
	ImGui::InputFloat(u("Bouncing"), &collider.bouncing, 0.1f, 1.f);
	ImGui::InputFloat(u("Friction"), &collider.friction, 0.1f, 1.f);
}

const char* ComponentNode<RigidBody2D>::name() { return "RigidBody2D"; }
const char* ComponentNode<RigidBody2D>::icon() { return ICON_FA_APPLE_ALT; }
void ComponentNode<RigidBody2D>::draw(RigidBody2D& rigid)
{
	UniqueID u(&rigid);
	ImGui::InputFloat(u("Mass"), &rigid.mass, 0.1f, 1.f);
	ImGui::InputFloat2(u("Acceleration"), rigid.acceleration.data);
	ImGui::InputFloat2(u("Velocity"), rigid.velocity.data);
}

const char* ComponentNode<Camera2D>::name() { return "Camera2D"; }
const char* ComponentNode<Camera2D>::icon() { return ICON_FA_CAMERA; }
void ComponentNode<Camera2D>::draw(Camera2D& camera)
{
	UniqueID u(&camera);
	ImGui::InputFloat2(u("Position"), camera.position.data);
	ImGui::InputFloat2(u("Viewport"), camera.viewport.data);
}
const char* ComponentNode<SoundInstance>::name() { return "SoundInstance"; }
const char* ComponentNode<SoundInstance>::icon() { return ICON_FA_MUSIC; }
void ComponentNode<SoundInstance>::draw(SoundInstance& sound)
{
	UniqueID u(&sound);
	ImGui::Text("Sound : %s", sound.path.c_str());
	ImGui::SliderFloat(u("Volume"), &sound.volume, 0.f, 2.f);
	ImGui::Checkbox(u("Loop"), &sound.loop);
}

const char* ComponentNode<Coin>::name() { return "Coin"; }
const char* ComponentNode<Coin>::icon() { return ICON_FA_COINS; }
void ComponentNode<Coin>::draw(Coin& coin)
{
	UniqueID u(&coin);
	ImGui::Checkbox(u("Picked"), &coin.picked);
}

const char* ComponentNode<Text>::name() { return "Text"; }
const char* ComponentNode<Text>::icon() { return ICON_FA_FONT; }
void ComponentNode<Text>::draw(Text& text)
{
	UniqueID u(&text);
	ImGui::InputFloat4(u("Color"), text.color.data);
	ImGui::SliderInt(u("Layer"), &text.layer, -20, 20);

	uint32_t currentHeight = text.font->height();
	const char* currentFont = text.font->family().c_str();
	static char buffer[256];
	snprintf(buffer, 256, "%s (%u)", text.font->family().c_str(), text.font->height());
	if (ImGui::BeginCombo("Font", buffer))
	{
		for (auto& font : Resources::font)
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
	if (ImGui::InputText(u("Text"), t, 256))
		text.text = t;
}

const char* ComponentNode<Player>::name() { return "Player"; }
const char* ComponentNode<Player>::icon() { return ICON_FA_RUNNING; }
void ComponentNode<Player>::draw(Player& player)
{
	UniqueID u(&player);
	ImGui::SliderInt(u("Coin"), &player.coin, 0, 50);
	float metric = player.speed.metric();
	if (ImGui::SliderFloat(u("Speed"), &metric, 0.f, 50.f))
		player.speed = Speed(metric);
	player.state;
	player.jump;
	player.left;
	player.right;
}

const char* ComponentNode<TileLayer>::name() { return "TileLayer"; }
const char* ComponentNode<TileLayer>::icon() { return ICON_FA_LAYER_GROUP; }
void ComponentNode<TileLayer>::draw(TileLayer& layer)
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
	ImGui::InputFloat4(u("Color"), layer.color.data);
}

const char* ComponentNode<TileMap>::name() { return "TileMap"; }
const char* ComponentNode<TileMap>::icon() { return ICON_FA_ATLAS; }
void ComponentNode<TileMap>::draw(TileMap& map)
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
				map.texture = Texture::create(image.width, image.height, Texture::Format::Rgba, image.bytes.data(), sampler);
			}
			catch (const std::exception&) {}
		}
	}
	else
	{
		float ratio = static_cast<float>(map.texture->width()) / static_cast<float>(map.texture->height());
		ImGui::Image((void*)map.texture->handle().value(), ImVec2(300, 300 * 1 / ratio), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1,1,1,1), ImVec4(0.5,0.5,0.5,1));
	}
}

// if return false, element deleted
template <typename T>
void component(Entity* entity)
{
	static char buffer[256];
	T* component = entity->get<T>();
	if (component != nullptr)
	{
		snprintf(buffer, 256, "%s %s##%p", ComponentNode<T>::icon(), ComponentNode<T>::name(), &component);
		if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			snprintf(buffer, 256, "ClosePopUp##%p", &component);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup(buffer);
			ComponentNode<T>::draw(*component);
			if (ImGui::BeginPopupContextItem(buffer))
			{
				if (ImGui::MenuItem("Remove"))
					entity->remove<T>();
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}
}

void EntityWidget::draw(World& world, Resources& resources)
{
	if (ImGui::Begin("Entities##window"))
	{
		ImVec4 color = ImVec4(236.f / 255.f, 11.f / 255.f, 67.f / 255.f, 1.f);
		static bool filterEntities = false;
		static const char noItem[256] = "All";
		static uint8_t componentID = 0;
		std::vector<const char*> items(Component::Type::count(), noItem);
		items[Component::Type::get<Transform2D>()] = ComponentNode<Transform2D>::name();
		items[Component::Type::get<Animator>()] = ComponentNode<Animator>::name();
		items[Component::Type::get<Player>()] = ComponentNode<Player>::name();
		items[Component::Type::get<Collider2D>()] = ComponentNode<Collider2D>::name();
		items[Component::Type::get<RigidBody2D>()] = ComponentNode<RigidBody2D>::name();
		items[Component::Type::get<Camera2D>()] = ComponentNode<Camera2D>::name();
		items[Component::Type::get<SoundInstance>()] = ComponentNode<SoundInstance>::name();
		items[Component::Type::get<Coin>()] = ComponentNode<Coin>::name();
		items[Component::Type::get<Text>()] = ComponentNode<Text>::name();
		items[Component::Type::get<TileLayer>()] = ComponentNode<TileLayer>::name();
		items[Component::Type::get<TileMap>()] = ComponentNode<TileMap>::name();

		ImGui::Checkbox("##EnableFilter", &filterEntities);
		ImGui::SameLine();
		if (ImGui::BeginCombo("Filter", items[componentID]))
		{
			for (uint8_t i = 0; i < (uint8_t)items.size(); i++)
			{
				bool same = (componentID == i);
				if (ImGui::Selectable(items[i], same))
				{
					if (!same)
						componentID = i;
				}
				if (same)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		uint32_t index = 0;
		if (ImGui::BeginChild("##list", ImVec2(0, -30), true))
		{
			world.each([&](Entity* entity) {
				UniqueID u(entity);
				index++;
				if (filterEntities && !entity->has(componentID))
					return;
				char buffer[256];
				snprintf(buffer, 256, "Entity %u", index);
				if (ImGui::CollapsingHeader(buffer))
				{
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
						ImGui::OpenPopup(u("ClosePopup"));
					if (ImGui::BeginPopupContextItem(u("ClosePopup")))
					{
						// Add component
						static int currentComponent = 0;
						if (ImGui::BeginMenu("Add component")) {
							if (ImGui::MenuItem(ComponentNode<Transform2D>::name(), nullptr, nullptr, !entity->has<Transform2D>()))
								entity->add<Transform2D>(Transform2D());
							else if (ImGui::MenuItem(ComponentNode<Animator>::name(), nullptr, nullptr, !entity->has<Animator>()))
								entity->add<Animator>(Animator(resources.sprite.getDefault(), 0));
							else if (ImGui::MenuItem(ComponentNode<Collider2D>::name(), nullptr, nullptr, !entity->has<Collider2D>()))
								entity->add<Collider2D>(Collider2D());
							else if (ImGui::MenuItem(ComponentNode<RigidBody2D>::name(), nullptr, nullptr, !entity->has<RigidBody2D>()))
								entity->add<RigidBody2D>(RigidBody2D());
							else if (ImGui::MenuItem(ComponentNode<Text>::name(), nullptr, nullptr, !entity->has<Text>()))
								entity->add<Text>(Text());
							else if (ImGui::MenuItem(ComponentNode<TileMap>::name(), nullptr, nullptr, !entity->has<TileMap>()))
								entity->add<TileMap>(TileMap());
							else if (ImGui::MenuItem(ComponentNode<TileLayer>::name(), nullptr, nullptr, !entity->has<TileLayer>()))
								entity->add<TileLayer>(TileLayer());
							else if (ImGui::MenuItem(ComponentNode<Coin>::name(), nullptr, nullptr, !entity->has<Coin>()))
								entity->add<Coin>(Coin());
							else if (ImGui::MenuItem(ComponentNode<Player>::name(), nullptr, nullptr, !entity->has<Player>()))
								entity->add<Player>(Player());
							else if (ImGui::MenuItem(ComponentNode<Camera2D>::name(), nullptr, nullptr, !entity->has<Camera2D>()))
								entity->add<Camera2D>(Camera2D());
							else if (ImGui::MenuItem(ComponentNode<SoundInstance>::name(), nullptr, nullptr, !entity->has<SoundInstance>()))
								entity->add<SoundInstance>(SoundInstance());
							ImGui::EndMenu();
						}
						//ImGui::SameLine();
						//ImGui::Combo("##Component", &currentComponent, items.data(), items.size());
						// Remove entity
						if (ImGui::MenuItem("Delete entity"))
							entity->destroy();
						ImGui::EndPopup();
					}
					component<Transform2D>(entity);
					component<SoundInstance>(entity);
					component<Collider2D>(entity);
					component<RigidBody2D>(entity);
					component<Camera2D>(entity);
					component<Animator>(entity);
					component<TileLayer>(entity);
					component<TileMap>(entity);
					component<Text>(entity);
					component<Player>(entity);
					component<Coin>(entity);
				}
			});
		}
		ImGui::EndChild();
		if (ImGui::Button("Add entity")) {
			world.createEntity();
		}
	}
	ImGui::End();
}

};