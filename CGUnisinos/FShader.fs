#version 450

in vec2 tex_coord_shader;

out vec4 color;

uniform sampler2D tex_buffer;

void main()
{
	color = texture(tex_buffer, tex_coord_shader);
};