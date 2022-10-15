#version 450

layout (location = 0) in vec2 a_position;

// Screen set
layout (set = 0, binding = 0) uniform ViewUniformBuffer
{
	vec2 u_resolution;
};
// Camera set
layout (set = 1, binding = 0) uniform CameraUniformBuffer
{
	mat4 u_view;
	mat4 u_projection;
};

struct TileInstanceUniformBuffer
{
	mat4 u_model;
	vec4 u_coordinates;
};
// Instance set
layout (set = 2, binding = 0) uniform TileInstanceUniformBuffers
{
	TileInstanceUniformBuffer u_instances[1024]; // TODO use dynamically sized array instead using SSBO.
};

layout (location = 0) out vec2 v_position;
layout (location = 1) out vec2 v_uv;

void main(void)
{
	vec2 start = u_instances[gl_InstanceIndex].u_coordinates.xy;
	vec2 end = u_instances[gl_InstanceIndex].u_coordinates.zw;
	v_position = a_position;
	v_uv = start + (end - start) * a_position;
#if defined(AKA_FLIP_UV)
	v_uv.y = 1.0 - v_uv.y;
#endif
	gl_Position = u_projection * u_view * u_instances[gl_InstanceIndex].u_model * vec4(a_position, 0.0, 1.0);
}
