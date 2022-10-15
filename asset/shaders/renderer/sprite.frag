#version 450

layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_uv;

// Instance UBO
layout (set = 2, binding = 0) uniform SpriteInstanceUniformBuffer
{
	mat4 u_model;
	uint u_layer;
};

#ifdef ANIMATED_SPRITE
layout(set = 2, binding = 1) uniform sampler2DArray u_texture;
#else
layout(set = 2, binding = 1) uniform sampler2D u_texture;
#endif

layout (location = 0) out vec4 o_color;

void main(void)
{
#ifdef ANIMATED_SPRITE
	vec4 color = texture(u_texture, vec3(v_uv, u_layer));
#else
	vec4 color = texture(u_texture, v_uv);
#endif
	if (color.a < 0.5)
		discard;
	o_color = color;
}
