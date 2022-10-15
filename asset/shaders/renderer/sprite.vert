#version 450

layout (location = 0) in vec2 a_position;

// View UBO
layout (set = 0, binding = 0) uniform ViewUniformBuffer
{
	vec2 u_resolution;
};
layout (set = 1, binding = 0) uniform CameraUniformBuffer
{
	mat4 u_view;
	mat4 u_projection;
};

// Instance UBO
layout (set = 2, binding = 0) uniform SpriteInstanceUniformBuffer
{
	mat4 u_model;
	uint u_layer;
};

layout (location = 0) out vec2 v_position;
layout (location = 1) out vec2 v_uv;

void main(void)
{
	v_position = a_position;
	v_uv = a_position;// * 0.5 + 0.5;
#if defined(AKA_FLIP_UV)
	v_uv.y = 1.0 - v_uv.y;
#endif
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 0.0, 1.0);
}
