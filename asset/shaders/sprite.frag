#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

vec2 getPixelPerfectUV(vec2 uv)
{
    vec2 size = vec2(320, 180);
    uv = floor(uv * size) + 0.5;
    
    // subpixel aa algorithm (COMMENT OUT TO COMPARE WITH POINT SAMPLING)
    uv += 1.0 - clamp((1.0 - fract(pixel)) * scale, 0.0, 1.0);

    // output
   	return uv / iChannelResolution[0].xy;
}

void main()
{
    // https://www.shadertoy.com/view/MlB3D3
    // emulate point sampling
    vec2 size = vec2(320, 180);
    vec2 uv = floor(TexCoords * size) + 0.5;
    
    // subpixel aa algorithm (COMMENT OUT TO COMPARE WITH POINT SAMPLING)
    uv += 1.0 - clamp((1.0 - fract(pixel)) * scale, 0.0, 1.0);

    // output
   	color = texture(iChannel0, getPixelPerfectUV(TexCoords));




    //color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
} 
