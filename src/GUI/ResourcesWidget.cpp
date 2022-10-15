#include "ResourcesWidget.h"

#include "../Component/SpriteAnimator.h"
#include "../Game/Resources.h"
#include "Modal.h"

namespace aka {

void ResourcesWidget::draw(World& world)
{
	if (ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_MenuBar))
	{
		static std::string error;
		if (ImGui::BeginMenuBar())
		{
			// All loading stuff goes there... few !
			if (ImGui::BeginMenu("Load"))
			{
				if (ImGui::MenuItem("Sprite"))
				{

				}
				if (ImGui::MenuItem("Audio"))
				{

				}
				if (ImGui::MenuItem("Font"))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::BeginTabBar("ResourcesTabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Fonts"))
			{
				if (ImGui::BeginChild("FontChild", ImVec2(0, 300), true))
				{
					AssetRegistry* registry = Application::app()->resource();
					for (auto asset : *registry)
					{
						if (asset.type != ResourceType::Font)
							continue;
						String name = asset.name;
						Font* font = asset.get<Font>();
						if (ImGui::TreeNode(name.cstr()))
						{
							ImGui::Text("Family : %s", font->family().cstr());
							ImGui::Text("Style : %s", font->style().cstr());
							ImGui::Text("Height : %upx", font->height());

							gfx::TextureHandle atlas = font->getCharacter(0).texture.texture;
							float uvx = 1.f / (atlas.data->height / font->height());
							float uvy = 1.f / (atlas.data->width / font->height());

							uint32_t lineCount = 0;
							for (uint32_t i = 0; i < (uint32_t)font->count(); i++)
							{
								const Character& character = font->getCharacter(i);
								ImGui::Image(
									(ImTextureID)character.texture.texture.data->native,
									ImVec2(30, 30),
#if defined(ORIGIN_BOTTOM_LEFT)
									ImVec2(character.texture.getStart().u, character.texture.getEnd().v + uvy),
									ImVec2(character.texture.getStart().u + uvx, character.texture.getEnd().v),
#else
									ImVec2(character.texture.getStart().u, character.texture.getEnd().v),
									ImVec2(character.texture.getStart().u + uvx, character.texture.getEnd().v + uvy),
#endif
									ImVec4(1, 1, 1, 1),
									ImVec4(1, 1, 1, 1)
								);
								if (ImGui::IsItemHovered())
								{
									ImGui::BeginTooltip();
									ImGui::Text("Advance : %u", character.advance);
									ImGui::Text("Size : (%d, %d)", character.size.x, character.size.y);
									ImGui::Text("Bearing : (%d, %d)", character.bearing.x, character.bearing.y);
									ImVec2 size = ImVec2(300, 300);
									ImGui::Image(
										(ImTextureID)character.texture.texture.data->native,
										size,
										ImVec2(0, 0),
										ImVec2(1, 1),
										ImVec4(1, 1, 1, 1),
										ImVec4(1, 1, 1, 1)
									);
									uv2f start = character.texture.getStart();
									uv2f end = character.texture.getEnd();
									ImVec2 startVec = ImVec2(ImGui::GetItemRectMin().x + start.u * size.x, ImGui::GetItemRectMin().y + start.v * size.y);
									ImVec2 endVec = ImVec2(ImGui::GetItemRectMin().x + end.u * size.x + 1, ImGui::GetItemRectMin().y + end.v * size.y + 1);
									ImU32 red = (93 << 24) | (4 << 16) | (26 << 8) | (255 << 0);
									ImGui::GetWindowDrawList()->AddRect(startVec, endVec, ImU32(red), 0.f, ImDrawCornerFlags_All, 2.f);
									ImGui::EndTooltip();
								}
								if (lineCount++ < 10)
									ImGui::SameLine();
								else
									lineCount = 0;
							}
							ImGui::TreePop();
						}
					}
				}
				ImGui::EndChild();
				if (ImGui::Button("Load font"))
					ImGui::OpenPopup("Font settings");
				bool opened = true;
				if (ImGui::BeginPopupModal("Font settings", &opened, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static Path path;
					static int height = 48;
					static char bufferPath[512];
					if (ImGui::InputTextWithHint("Path##Font", "Path to font", bufferPath, 512))
						path = bufferPath;
					ImGui::SameLine();
					if (Modal::LoadButton("Browse", &path))
						String::copy(bufferPath, 256, path.cstr());
					ImGui::SliderInt("Height##Font", &height, 1, 200);

					if (ImGui::Button("OK"))
					{
						if (strlen(bufferPath) == 0)
						{
							error = "No path for font.";
						}
						else
						{
							/*try
							{
								Blob blob;
								if (!OS::File::read(path, &blob))
									Logger::error("Failed to load font");
								FontManager::create(OS::File::name(path), Font::create(blob.data(), blob.size(), height));
								String::copy(bufferPath, 256, path.cstr());
								height = 48;
								path = "";
								ImGui::CloseCurrentPopup();
							}
							catch (const std::exception&)
							{
								error = "Failed loading font.";
							}*/
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
					}
					Modal::Error("Error", error);
					ImGui::EndPopup();
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Sprites"))
			{
				if (ImGui::BeginChild("SpriteChild", ImVec2(0, 300), true))
				{
					AssetRegistry* registry = Application::app()->resource();
					for (auto asset : *registry)
					{
						if (asset.type != ResourceType::Sprite)
							continue;
						String name = asset.name;
						Sprite* sprite = asset.get<Sprite>();
						if (ImGui::TreeNode(name.cstr()))
						{
							for (uint32_t i = 0; i < sprite->getAnimationCount(); i++)
							{
								SpriteAnimation& animation = sprite->getAnimation(i);
								if (ImGui::TreeNodeEx(animation.name.cstr()))
								{
									char buffer[256];
									String::copy(buffer, 256, animation.name.cstr());
									if (ImGui::InputText("Name", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
										animation.name = buffer;
									uint32_t frameID = 0;
									for (SpriteFrame& frame : animation.frames)
									{
										int error = snprintf(buffer, 256, "Frame %u", frameID++);
										if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Bullet))
										{
											int d = (int)frame.duration.milliseconds();
											if (ImGui::SliderInt("Duration", &d, 0, 1000))
											{
												frame.duration = Time::milliseconds(d);
												auto view = world.registry().view<SpriteAnimatorComponent>();
												view.each([sprite](SpriteAnimatorComponent& animator) {
													// Reset current frame duration stored in animator
													if (animator.sprite == sprite)
														animator.update();
												});
											}
											int size[2]{ (int)frame.width, (int)frame.height };
											if (ImGui::InputInt2("Size", size))
											{
												if (size[0] < 1) size[0] = 1;
												if (size[1] < 1) size[1] = 1;
												frame.width = (uint32_t)size[0];
												frame.height = (uint32_t)size[1];
											}
											float ratio = static_cast<float>(frame.handle.data->width) / static_cast<float>(frame.handle.data->height);
											ImGui::Image(
												(ImTextureID)frame.handle.data->native,
												ImVec2(200, 200 * 1 / ratio),
#if defined(ORIGIN_BOTTOM_LEFT)
												ImVec2(0, 1), ImVec2(1, 0)
#else
												ImVec2(0, 0), ImVec2(1, 1)
#endif
											);
											ImGui::TreePop();
										}
									}
									Path path;
									if (Modal::LoadButton("Load image", &path))
									{
										Image image = Image::load(path);
										if (image.size() == 0)
										{
											error = "Could not load image";
										}
										else
										{
											// TODO texture as resource ?
											// TODO modal to select sampler
											/*SpriteFrame frame = SpriteFrame::create(
												gfx::Texture::create2D(image.width(), image.height(), TextureFormat::RGBA8, TextureFlag::ShaderResource),
												Time::milliseconds(500)
											);
											frame.texture->upload(image.data());
											animation.frames.push_back(frame);*/
										}
									}
									Modal::Error("Error", error);
									ImGui::TreePop();
								}
							}
							static char buffer[256];
							ImGui::InputText("##Name", buffer, 256);
							ImGui::SameLine();
							if (ImGui::Button("Add animation") && strlen(buffer) > 0)
							{
								SpriteAnimation animation;
								animation.name = buffer;
								//sprite->animations.push_back(animation);
							}

							ImGui::TreePop();
						}
					}
				}
				ImGui::EndChild();
				if (ImGui::Button("Load sprite"))
					ImGui::OpenPopup("Sprite loader");
				ImGui::SameLine();
				if (ImGui::Button("Add sprite"))
					ImGui::OpenPopup("Sprite settings");
				bool opened = true;
				if (ImGui::BeginPopupModal("Sprite settings", &opened, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static char spriteName[256];
					ImGui::InputTextWithHint("Name##Sprite", "Sprite name", spriteName, 256);
					/*if (ImGui::Button("Create##Sprite"))
					{
						SpriteManager::create(spriteName, Sprite());
						ImGui::CloseCurrentPopup();
					}*/
					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}
				bool openedLoader = true;
				if (ImGui::BeginPopupModal("Sprite loader", &openedLoader, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static Path path;
					static char bufferPath[512];
					if (ImGui::InputTextWithHint("Path##SpriteLoad", "Path to aseprite file", bufferPath, 512))
						path = bufferPath;
					ImGui::SameLine();
					if (Modal::LoadButton("Browse##SpriteLoader", &path))
						String::copy(bufferPath, 512, path.cstr());
					if (ImGui::Button("OK##SpriteLoader"))
					{
						if (strlen(bufferPath) == 0)
						{
							error = "No path for aseprite file.";
						}
						else
						{
							/*try
							{
								FileStream stream(path, FileMode::Read, FileType::Binary);
								SpriteManager::create(OS::File::name(bufferPath), Sprite::parse(stream));
								String::copy(bufferPath, 256, path.cstr());
								path = "";
								ImGui::CloseCurrentPopup();
							}
							catch (const std::exception&)
							{
								error = "Failed to load aseprite file.";
							}*/
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel##SpriteLoader"))
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Audios"))
			{
				if (ImGui::BeginChild("AudioChild", ImVec2(0, 300), true))
				{
					AssetRegistry* registry = Application::app()->resource();
					for (auto asset : *registry)
					{
						if (asset.type != ResourceType::Audio)
							continue;
						String name = asset.name;
						//AudioStream* audio = asset.get<AudioStream>();
					
						if (ImGui::TreeNode(name.cstr()))
						{
							float duration = 0.f;// audio->samples() / (float)(audio->frequency() * audio->channels());
							uint32_t ms = (uint32_t)(duration * 1000.f) % 1000;
							uint32_t s = (uint32_t)(duration) % 60;
							uint32_t m = (uint32_t)(duration / 60);
							uint32_t h = (uint32_t)(duration / (60 * 60));
							ImGui::Text("Duration : %02u:%02u:%02u.%03u", h, m, s, ms);
							//ImGui::Text("Frequency : %u", audio->frequency());
							//ImGui::Text("Channels : %u", audio->channels());
							ImGui::TreePop();
						}
					}
				}
				ImGui::EndChild();
				if (ImGui::Button("Load Audio"))
					ImGui::OpenPopup("Audio settings");
				bool opened = true;
				if (ImGui::BeginPopupModal("Audio settings", &opened, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static Path path;
					static bool memory = false;
					static char bufferPath[512];
					if (ImGui::InputTextWithHint("Path##Audio", "Path to audio", bufferPath, 512))
						path = bufferPath;
					ImGui::SameLine();
					if (Modal::LoadButton("Browse", &path))
						String::copy(bufferPath, 256, path.cstr());
					ImGui::Checkbox("Memory", &memory);

					if (ImGui::Button("OK"))
					{
						if (strlen(bufferPath) == 0)
						{
							error = "No path for audio.";
						}
						else
						{
							AudioStream* stream;
							if (memory)
								stream = AudioStream::loadMemory(path);
							else
								stream = AudioStream::openStream(path);
							if (stream == nullptr)
								error = "Could not load audio";
							/*else
							{
								AudioManager::create(OS::File::name(path), std::move(stream));
								String::copy(bufferPath, 256, path.cstr());
								path = "";
								memory = false;
								ImGui::CloseCurrentPopup();
							}*/
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
					}
					Modal::Error("Error", error);
					ImGui::EndPopup();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

};
