#pragma once

#include <Aka/Scene/System.h>
#include <Aka/Graphic/Shader.h>
#include <Aka/Resource/Resource/StaticMesh.h>

namespace aka {

struct alignas(16) CameraUniformBuffer
{
	mat4f view;
	mat4f projection;
};

struct alignas(16) SpriteInstanceUniformBuffer
{
	mat4f model;
	uint32_t layer; // layer on texture.
};

struct alignas(16) TileInstanceUniformBuffer
{
	mat4f model;
	vec4f coordinates;
};

struct alignas(16) TargetUniformBuffer
{
	vec2u resolution;
};

struct SpriteRenderDataComponent
{
	gfx::BufferHandle buffer;
	gfx::SamplerHandle sampler;
	gfx::DescriptorSetHandle descriptorSet;
};

struct TileMapRenderDataComponent
{
	gfx::BufferHandle buffer;
	gfx::SamplerHandle sampler;
	gfx::DescriptorSetHandle descriptorSet;
};

class RenderSystem : public System
{
public:
	static inline const vec2u resolution = vec2u(320U, 180U);
	static inline const vec2u gridSize = vec2u(16, 16);

public:
	void onCreate(World& world) override;
	void onDestroy(World& world) override;

	void onFixedUpdate(World& world, Time deltaTime)  override;
	void onUpdate(World& world, Time deltaTime) override;
	void onRender(World& world, gfx::Frame* frame)  override;

	void onResize(World& world, uint32_t width, uint32_t height) override;
private:
	// --- Render target
	// Description
	gfx::VertexAttributeState m_vertexAttributes;
	gfx::FramebufferState m_framebufferState;
	// Animated sprite pipeline
	gfx::GraphicPipelineHandle m_animatedSpritePipeline;
	// Static sprite pipeline
	gfx::GraphicPipelineHandle m_staticSpritePipeline;
	// Tilemap pipeline
	gfx::GraphicPipelineHandle m_tilemapPipeline;
	// Rendering target
	gfx::FramebufferHandle m_framebuffer;
	gfx::TextureHandle m_target;
	gfx::TextureHandle m_targetDepth;
	// --- Meshes
	StaticMesh* m_quad;
	gfx::DescriptorSetHandle m_targetDescriptorSet;
	gfx::DescriptorSetHandle m_cameraDescriptorSet;
	gfx::BufferHandle m_targetUniformBuffer;
	gfx::BufferHandle m_cameraUniformBuffer;
};

}
