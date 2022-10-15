#version 450

layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_uv;

layout(set = 2, binding = 1) uniform sampler2D u_texture;

layout (location = 0) out vec4 o_color;

void main(void)
{
	vec4 color = texture(u_texture, v_uv);
	if (color.a < 0.5)
		discard;
	o_color = color;
}
