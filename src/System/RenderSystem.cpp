#include "RenderSystem.h"

#include <Aka/OS/OS.h>
#include <Aka/Scene/World.h>

#include "../Shaders.h"

#include "../Game/Views/GameView.h"

#include "../Component/Camera2D.h"
#include "../Component/Transform2D.h"
#include "../Component/SpriteAnimator.h"
#include "../Component/TileMap.h"
#include "../Component/TileLayer.h"
#include "../Component/Text.h"
#include "../Component/Sprite.h"

namespace aka {

void onSpriteRenderDataRemoved(entt::registry& registry, entt::entity entity)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();
	SpriteRenderDataComponent& data = registry.get<SpriteRenderDataComponent>(entity);
	device->destroy(data.descriptorSet);
	device->destroy(data.buffer);
	device->destroy(data.sampler);
}
void onTilemapRenderDataRemoved(entt::registry& registry, entt::entity entity)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();
	TileMapRenderDataComponent& data = registry.get<TileMapRenderDataComponent>(entity);
	device->destroy(data.descriptorSet);
	device->destroy(data.buffer);
	device->destroy(data.sampler);
}
void onTileMapRemoved(entt::registry& registry, entt::entity entity)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();
	TileMapComponent& data = registry.get<TileMapComponent>(entity);
	device->destroy(data.texture);
}

void RenderSystem::onCreate(World& world)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();
	ShaderRegistry* registry = app->program();
	PlatformDevice* platform = app->platform();

	// Setup render targets
	uint32_t width = (uint32_t)(platform->width() * RenderSystem::resolution.y / (float)platform->height());
	uint32_t height = RenderSystem::resolution.y;
	m_target = device->createTexture(
		width,
		height,
		1,
		gfx::TextureType::Texture2D,
		1,
		1,
		gfx::TextureFormat::RGBA8,
		gfx::TextureFlag::RenderTarget | gfx::TextureFlag::ShaderResource
	);
	m_targetDepth = device->createTexture(
		width,
		height,
		1,
		gfx::TextureType::Texture2D,
		1,
		1,
		gfx::TextureFormat::Depth16,
		gfx::TextureFlag::RenderTarget
	);
	gfx::Attachment targetAttachment = gfx::Attachment{
		m_target,
		gfx::AttachmentFlag::None,
		gfx::AttachmentLoadOp::Clear,
		0,
		0
	};
	gfx::Attachment targetDepthAttachment = gfx::Attachment{
		m_targetDepth,
		gfx::AttachmentFlag::None,
		gfx::AttachmentLoadOp::Clear,
		0,
		0
	};
	m_framebuffer = device->createFramebuffer(&targetAttachment, 1, &targetDepthAttachment);
	// TODO device->getFramebufferState(framebuffer);
	m_framebufferState = {};
	m_framebufferState.colors[0].format = targetAttachment.texture.data->format;
	m_framebufferState.colors[0].loadOp = targetAttachment.loadOp;
	m_framebufferState.depth.format = targetDepthAttachment.texture.data->format;
	m_framebufferState.depth.loadOp = targetDepthAttachment.loadOp;
	m_framebufferState.count = 1;

	// Attributes
	m_vertexAttributes = {};
	m_vertexAttributes.count = 1;
	m_vertexAttributes.attributes[0] = gfx::VertexAttribute{ gfx::VertexSemantic::Position, gfx::VertexFormat::Float, gfx::VertexType::Vec2 };

	// Viewport
	gfx::ViewportState viewportState{
		Rect{ 0, 0, resolution.x, resolution.y },
		Rect{ 0, 0, resolution.x, resolution.y }
	};

	// Animated sprite pipeline
	gfx::ProgramHandle animatedSpriteProgram = registry->get(ProgramAnimatedSprite);
	m_animatedSpritePipeline = device->createGraphicPipeline(
		animatedSpriteProgram,
		gfx::PrimitiveType::Triangles,
		m_framebufferState,
		m_vertexAttributes,
		viewportState,
		gfx::DepthStateLess,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStatePremultiplied,
		gfx::FillStateFill
	);

	// Static sprite pipeline
	gfx::ProgramHandle staticSpriteProgram = registry->get(ProgramStaticSprite);
	m_staticSpritePipeline = device->createGraphicPipeline(
		staticSpriteProgram,
		gfx::PrimitiveType::Triangles,
		m_framebufferState,
		m_vertexAttributes,
		viewportState,
		gfx::DepthStateLess,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStatePremultiplied,
		gfx::FillStateFill
	);

	// Tilemap pipeline
	gfx::ProgramHandle tilemapProgram = app->program()->get(ProgramTilemap);
	m_tilemapPipeline = device->createGraphicPipeline(
		tilemapProgram,
		gfx::PrimitiveType::Triangles,
		m_framebufferState,
		m_vertexAttributes,
		viewportState,
		gfx::DepthStateLess,
		gfx::StencilStateDisabled,
		gfx::CullStateDisabled,
		gfx::BlendStatePremultiplied,
		gfx::FillStateFill
	);

	// UBO
	TargetUniformBuffer targetUBO{};
	targetUBO.resolution = resolution;
	m_targetUniformBuffer = device->createBuffer(gfx::BufferType::Uniform, sizeof(TargetUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, &targetUBO);
	m_cameraUniformBuffer = device->createBuffer(gfx::BufferType::Uniform, sizeof(CameraUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None);
	//gfx::ShaderBindingState state; // TODO
	m_targetDescriptorSet = device->createDescriptorSet(animatedSpriteProgram.data->sets[0]);
	m_cameraDescriptorSet = device->createDescriptorSet(animatedSpriteProgram.data->sets[1]);

	gfx::DescriptorSetData targetData{};
	targetData.setUniformBuffer(0, m_targetUniformBuffer);
	device->update(m_targetDescriptorSet, targetData);

	gfx::DescriptorSetData cameraData{};
	cameraData.setUniformBuffer(0, m_cameraUniformBuffer);
	device->update(m_cameraDescriptorSet, cameraData);

	// Quad mesh
	float quadVertices[] = {
		0.f, 0.f, // bottom left corner
		 1.f, 0.f, // bottom right corner
		 1.f,  1.f, // top right corner
		0.f,  1.f, // top left corner
	};
	uint16_t quadIndices[] = { 0,1,2,0,2,3 };
	m_quad = StaticMesh::createInterleaved(m_vertexAttributes, quadVertices, 4, gfx::IndexFormat::UnsignedShort, quadIndices, 6);
	m_quad->createRenderData(device, m_quad->getBuildData());
	// Clear render component
	world.registry().on_destroy<SpriteRenderDataComponent>().connect<&onSpriteRenderDataRemoved>();
	world.registry().on_destroy<TileMapRenderDataComponent>().connect<&onTilemapRenderDataRemoved>();
	world.registry().on_destroy<TileMapComponent>().connect<&onTileMapRemoved>();
}

void RenderSystem::onDestroy(World& world)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();

	m_quad->destroyRenderData(device);
	device->destroy(m_targetDescriptorSet);
	device->destroy(m_cameraDescriptorSet);
	device->destroy(m_targetUniformBuffer);
	device->destroy(m_cameraUniformBuffer);
	device->destroy(m_animatedSpritePipeline);
	device->destroy(m_staticSpritePipeline);
	device->destroy(m_tilemapPipeline);
	device->destroy(m_framebuffer);
	device->destroy(m_target);
	device->destroy(m_targetDepth);

	delete m_quad;
}

void RenderSystem::onFixedUpdate(World& world, Time deltaTime)
{
}

void RenderSystem::onUpdate(World& world, Time deltaTime)
{
}

void RenderSystem::onRender(World& world, gfx::Frame* frame)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();
	gfx::CommandList* cmd = frame->commandList;

	// --- Create renderer data
	{
		// Sprite animated render data
		auto spriteAnimatedView = world.registry().view<SpriteAnimatorComponent>(entt::exclude<SpriteRenderDataComponent>);
		for (entt::entity e : spriteAnimatedView)
		{
			SpriteAnimatorComponent& animator = world.registry().get<SpriteAnimatorComponent>(e);
			SpriteRenderDataComponent& data = world.registry().emplace<SpriteRenderDataComponent>(e);
			gfx::ProgramHandle handle = Application::app()->program()->get(ProgramAnimatedSprite);
			data.descriptorSet = device->createDescriptorSet(handle.data->sets[2]);
			data.buffer = device->createBuffer(gfx::BufferType::Uniform, sizeof(SpriteInstanceUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None);
			data.sampler = device->createSampler(
				gfx::Filter::Nearest, gfx::Filter::Nearest,
				gfx::SamplerMipMapMode::None, 1,
				gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
				1.f
			);
			gfx::DescriptorSetData instanceData{};
			instanceData.setUniformBuffer(0, data.buffer);
			// TODO add support for image view. This would allow to merge both animated & non animated sprite shader & pipeline instead of duplicating them
			instanceData.setSampledImage(1, animator.sprite->getTextureHandle(), data.sampler);
			device->update(data.descriptorSet, instanceData);
		}
		// Sprite static render data
		auto spriteView = world.registry().view<SpriteComponent>(entt::exclude<SpriteRenderDataComponent>);
		for (entt::entity e : spriteView)
		{
			SpriteComponent& sprite = world.registry().get<SpriteComponent>(e);
			SpriteRenderDataComponent& data = world.registry().emplace<SpriteRenderDataComponent>(e);
			gfx::ProgramHandle handle = Application::app()->program()->get(ProgramStaticSprite);
			data.descriptorSet = device->createDescriptorSet(handle.data->sets[2]);
			data.buffer = device->createBuffer(gfx::BufferType::Uniform, sizeof(SpriteInstanceUniformBuffer), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None);
			data.sampler = device->createSampler(
				gfx::Filter::Nearest, gfx::Filter::Nearest,
				gfx::SamplerMipMapMode::None, 1,
				gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
				1.f
			);
			gfx::DescriptorSetData instanceData{};
			instanceData.setUniformBuffer(0, data.buffer);
			instanceData.setSampledImage(1, sprite.sprite->handle(), data.sampler);
			device->update(data.descriptorSet, instanceData);
		}
		// Tile map render data
		auto tileView = world.registry().view<TileMapComponent>(entt::exclude<TileMapRenderDataComponent>);
		for (entt::entity e : tileView)
		{
			TileMapComponent& tilemap = world.registry().get<TileMapComponent>(e);
			TileMapRenderDataComponent& data = world.registry().emplace<TileMapRenderDataComponent>(e);
			gfx::ProgramHandle handle = Application::app()->program()->get(ProgramTilemap);
			data.descriptorSet = device->createDescriptorSet(handle.data->sets[2]);
			uint32_t instanceCount = tilemap.gridCount.x * tilemap.gridCount.y;
			data.buffer = device->createBuffer(
				gfx::BufferType::Uniform, 
				sizeof(TileInstanceUniformBuffer) * instanceCount,
				gfx::BufferUsage::Default, 
				gfx::BufferCPUAccess::None
			);
			data.sampler = device->createSampler(
				gfx::Filter::Nearest, gfx::Filter::Nearest,
				gfx::SamplerMipMapMode::None, 1,
				gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
				1.f
			);
			gfx::DescriptorSetData instanceData{};
			instanceData.setUniformBuffer(0, data.buffer);
			instanceData.setSampledImage(1, tilemap.texture, data.sampler);
			device->update(data.descriptorSet, instanceData);
		}
		// Text render data
		auto textView = world.registry().view<Text2DComponent>(entt::exclude<TileMapRenderDataComponent>);
		for (entt::entity e : textView)
		{
			Text2DComponent& text = world.registry().get<Text2DComponent>(e);
			TileMapRenderDataComponent& data = world.registry().emplace<TileMapRenderDataComponent>(e);
			gfx::ProgramHandle handle = Application::app()->program()->get(ProgramTilemap);
			data.descriptorSet = device->createDescriptorSet(handle.data->sets[2]);
			uint32_t instanceCount = (uint32_t)encoding::length(text.text);
			// TODO update when text change, instances might increase.
			data.buffer = device->createBuffer(
				gfx::BufferType::Uniform,
				sizeof(TileInstanceUniformBuffer) * instanceCount,
				gfx::BufferUsage::Default,
				gfx::BufferCPUAccess::None
			);
			data.sampler = device->createSampler(
				gfx::Filter::Nearest, gfx::Filter::Nearest,
				gfx::SamplerMipMapMode::None, 1,
				gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
				1.f
			);
			gfx::DescriptorSetData instanceData{};
			instanceData.setUniformBuffer(0, data.buffer);
			instanceData.setSampledImage(1, text.font->atlas(), data.sampler);
			device->update(data.descriptorSet, instanceData);
		}
	}

	// Update camera UBO
	//if (dirty)
	{
		Entity cameraEntity = getMainCamera2DEntity(world);
		Transform2DComponent& cameraTransform = cameraEntity.get<Transform2DComponent>();
		Camera2DComponent& camera = cameraEntity.get<Camera2DComponent>();
		const mat3f cameraModelMatrix = cameraTransform.model();
		const mat3f viewMatrix = mat3f::inverse(cameraModelMatrix);
		CameraUniformBuffer cameraUBO{};
		cameraUBO.projection = camera.camera.projection();
		cameraUBO.view = mat4f::from2D(viewMatrix);
		device->upload(m_cameraUniformBuffer, &cameraUBO, 0, sizeof(CameraUniformBuffer));
	}


	cmd->beginRenderPass(m_framebuffer, gfx::ClearState{ gfx::ClearMask::All, { 1.f, 0.63f, 0.f, 1.f }, 1.f, 1 });
	m_quad->bind(cmd);

	// --- Render animated sprites
	cmd->bindPipeline(m_animatedSpritePipeline);
	auto spriteAnimatedView = world.registry().view<SpriteAnimatorComponent, Transform2DComponent, SpriteRenderDataComponent>();
	spriteAnimatedView.each([&](SpriteAnimatorComponent& animator, Transform2DComponent& transform, SpriteRenderDataComponent& renderData)
	{
		//if (dirty)
		{
			AKA_ASSERT(animator.layerDepth < Camera2DComponent::range&& animator.layerDepth > -Camera2DComponent::range, "Layer not in range");
			const SpriteFrame& currentFrame = animator.getCurrentSpriteFrame();
			const gfx::TextureHandle texture = currentFrame.handle;
			mat4f spriteScale = mat4f::scale(vec3f((float)currentFrame.width, (float)currentFrame.height, 1.f));
			mat4f spriteLayerDepth = mat4f::translate(vec3f(0.f, 0.f, (float)animator.layerDepth));
			mat4f spritemodel = mat4f::from2D(transform.model()) * spriteLayerDepth * spriteScale;
			SpriteInstanceUniformBuffer instanceUBO{};
			instanceUBO.model = spritemodel;
			instanceUBO.layer = animator.getCurrentSpriteFrame().layer;
			device->upload(renderData.buffer, &instanceUBO, 0, sizeof(SpriteInstanceUniformBuffer));
		}

		gfx::DescriptorSetHandle handles[3] = { m_targetDescriptorSet, m_cameraDescriptorSet, renderData.descriptorSet };
		cmd->bindDescriptorSets(handles, 3);
		// Bind descriptors
		m_quad->drawIndexed(cmd);
	});
	// --- Render static sprites
	cmd->bindPipeline(m_staticSpritePipeline);
	auto spriteStaticView = world.registry().view<SpriteComponent, Transform2DComponent, SpriteRenderDataComponent>();
	spriteStaticView.each([&](SpriteComponent& sprite, Transform2DComponent& transform, SpriteRenderDataComponent& renderData)
	{
		//if (dirty)
		{
			AKA_ASSERT(sprite.layerDepth < Camera2DComponent::range&& sprite.layerDepth > -Camera2DComponent::range, "Layer not in range");
			const gfx::TextureHandle texture = sprite.sprite->handle();
			mat4f spriteScale = mat4f::scale(vec3f((float)texture.data->width, (float)texture.data->height, 1.f));
			mat4f spriteLayerDepth = mat4f::translate(vec3f(0.f, 0.f, (float)sprite.layerDepth));
			mat4f spritemodel = mat4f::from2D(transform.model()) * spriteLayerDepth * spriteScale;
			SpriteInstanceUniformBuffer instanceUBO{};
			instanceUBO.model = spritemodel;
			instanceUBO.layer = 0;
			device->upload(renderData.buffer, &instanceUBO, 0, sizeof(SpriteInstanceUniformBuffer));
		}

		gfx::DescriptorSetHandle handles[3] = { m_targetDescriptorSet, m_cameraDescriptorSet, renderData.descriptorSet };
		cmd->bindDescriptorSets(handles, 3);
		// Bind descriptors
		m_quad->drawIndexed(cmd);
	});
	// --- Render tilemaps
	cmd->bindPipeline(m_tilemapPipeline);
	auto tileView = world.registry().view<Transform2DComponent, TileMapComponent, TileLayerComponent, TileMapRenderDataComponent>();
	tileView.each([&](Transform2DComponent& transform, TileMapComponent& atlas, TileLayerComponent& layer, TileMapRenderDataComponent& renderData)
	{
		if (atlas.texture == gfx::TextureHandle::null)
			return;
		AKA_ASSERT(layer.gridSize == atlas.gridSize, "");
		uint32_t instanceCount = static_cast<uint32_t>(layer.tileID.size());
		AKA_ASSERT(instanceCount < 1024, "More tile than max supported.");
		//if (dirty)
		{
			AKA_ASSERT(layer.layerDepth < Camera2DComponent::range&& layer.layerDepth > -Camera2DComponent::range, "Layer not in range");
			Vector<TileInstanceUniformBuffer> instanceUBOs(instanceCount);
			for (size_t i = 0; i < layer.tileID.size(); i++)
			{
				// Ogmo tileID is top left to bottom while opengl projection is bottom to top.
				vec2u tileID = vec2u((uint32_t)i % layer.gridCount.x, layer.gridCount.y - 1 - (uint32_t)i / layer.gridCount.x);
				mat4f tileOffset = mat4f::translate(vec3f(layer.offset + vec2f(tileID * layer.gridSize), 0.f));
				mat4f tileScale = mat4f::scale(vec3f(vec2f(layer.gridSize), 1.f));
				mat4f tileLayerDepth = mat4f::translate(vec3f(0.f, 0.f, (float)layer.layerDepth));
				mat4f tileModel = mat4f::from2D(transform.model()) * tileOffset * tileLayerDepth * tileScale;
				int32_t atlasIDUnique = layer.tileID[i];
				if (atlasIDUnique == -1)
					continue;
				vec2u atlasID = vec2u(atlasIDUnique % atlas.gridCount.x, atlas.gridCount.y - 1 - atlasIDUnique / atlas.gridCount.x);
				uv2f start = uv2f(vec2f(atlasID) / vec2f(atlas.gridCount));
				uv2f end = start + uv2f(vec2f(1.f) / vec2f(atlas.gridCount));

				instanceUBOs[i].model = tileModel;
				instanceUBOs[i].coordinates = vec4f(start.u, start.v, end.u, end.v);
			}
			device->upload(renderData.buffer, instanceUBOs.data(), 0, sizeof(SpriteInstanceUniformBuffer)* instanceCount);
		}

		gfx::DescriptorSetHandle handles[3] = { m_targetDescriptorSet, m_cameraDescriptorSet, renderData.descriptorSet };
		cmd->bindDescriptorSets(handles, 3);

		 m_quad->drawIndexed(cmd, instanceCount);
	});
	// --- Render text
    auto view = world.registry().view<Text2DComponent, Transform2DComponent, TileMapRenderDataComponent>();
	cmd->bindPipeline(m_tilemapPipeline); // We can use the same pipeline as tilemaps
    view.each([&](Text2DComponent& text, Transform2DComponent& transform, TileMapRenderDataComponent& renderData)
	{
		// TODO OPTI batch all text of the same fonts all together
		uint32_t instanceCount = 0;
		//if (dirty)
		{
			Vector<TileInstanceUniformBuffer> instanceUBOs;
			mat4f textLayerDepth = mat4f::translate(vec3f(0.f, 0.f, (float)text.layerDepth));
			float scale = 1.f;
			float advance = 0.f;
			const char* start = text.text.begin();
			const char* end = text.text.end();
			while (start < end)
			{
				uint32_t c = encoding::next(start, end);
				// TODO check if rendering text out of screen for culling ?
				const Character& ch = text.font->getCharacter(c);
				mat4f textScale = mat4f::scale(vec3f(vec2f((float)ch.size.x, (float)ch.size.y) * scale, 1.f));
				mat4f textOffset = mat4f::scale(vec3f(text.offset + vec2f(advance + ch.bearing.x, (float)-(ch.size.y - ch.bearing.y)) * scale, 1.f));
				mat4f textModel = mat4f::from2D(transform.model()) * textOffset * textLayerDepth * textScale;
				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				advance += ch.advance * scale;

				uv2f start = ch.texture.getStart();
				uv2f end = ch.texture.getEnd();
				instanceUBOs.append(TileInstanceUniformBuffer{
					textModel,
					vec4f(start.u, start.v, end.u, end.v)
				});
			}
			size_t size = encoding::length(text.text);
			instanceCount = (uint32_t)instanceUBOs.size();
			AKA_ASSERT(size == instanceCount, "Invalid size.");
			AKA_ASSERT(instanceCount < 1024, "More tile than max supported.");
			device->upload(renderData.buffer, instanceUBOs.data(), 0, sizeof(SpriteInstanceUniformBuffer)* instanceCount);
		}

		gfx::DescriptorSetHandle handles[3] = { m_targetDescriptorSet, m_cameraDescriptorSet, renderData.descriptorSet };
		cmd->bindDescriptorSets(handles, 3);

		m_quad->drawIndexed(cmd, instanceCount);
    });
	cmd->endRenderPass();

	// TODO some post pro ?

	// --- Blit target to backbuffer
	{
		gfx::FramebufferHandle backbuffer = device->backbuffer(frame);
		gfx::BlitRegion srcRegion{};
		srcRegion.w = resolution.x;
		srcRegion.h = resolution.y;
		srcRegion.d = 1;
		srcRegion.layerCount = 1;

		gfx::BlitRegion dstRegion{};
		dstRegion.w = backbuffer.data->width;
		dstRegion.h = backbuffer.data->height;
		dstRegion.d = 1;
		dstRegion.layerCount = 1;
		cmd->blit(m_target, backbuffer.data->colors[0].texture, srcRegion, dstRegion, gfx::Filter::Nearest);
	}
}

void RenderSystem::onResize(World& world, uint32_t width, uint32_t height)
{
	Application* app = Application::app();
	gfx::GraphicDevice* device = app->graphic();

	uint32_t newWidth = (uint32_t)(width * resolution.y / (float)height);
	uint32_t newHeight = resolution.y;

	device->destroy(m_target);
	device->destroy(m_targetDepth);
	device->destroy(m_framebuffer);

	m_target = device->createTexture(
		newWidth,
		newHeight,
		1,
		gfx::TextureType::Texture2D,
		1,
		1,
		gfx::TextureFormat::RGBA8,
		gfx::TextureFlag::RenderTarget | gfx::TextureFlag::ShaderResource
	);
	m_targetDepth = device->createTexture(
		width,
		height,
		1,
		gfx::TextureType::Texture2D,
		1,
		1,
		gfx::TextureFormat::Depth16,
		gfx::TextureFlag::RenderTarget
	);
	gfx::Attachment targetAttachment = gfx::Attachment{
		m_target,
		gfx::AttachmentFlag::None,
		gfx::AttachmentLoadOp::Clear,
		0,
		0
	};
	gfx::Attachment targetDepthAttachment = gfx::Attachment{
		m_targetDepth,
		gfx::AttachmentFlag::None,
		gfx::AttachmentLoadOp::Clear,
		0,
		0
	};
	m_framebuffer = device->createFramebuffer(&targetAttachment, 1, &targetDepthAttachment);
}

}
