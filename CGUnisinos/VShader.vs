#version 450
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coord_shader;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
    tex_coord_shader = vec2(tex_coord.x, 1 - tex_coord.y);
}