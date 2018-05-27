#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture_coords;

out vec4 block_color;
out vec2 tex_coords;

uniform int SCREEN_WIDTH;
uniform int SCREEN_HEIGHT;
uniform int BLOCK_SIZE;

void main() {
    vec2 processed_pos;
    processed_pos.x = float(pos.x * BLOCK_SIZE) / float(SCREEN_WIDTH) * 2.0 - 1.0;
    processed_pos.y = 1.0 - (float(pos.y * BLOCK_SIZE) / float(SCREEN_HEIGHT) * 2.0);

	gl_Position = vec4(processed_pos, 0.0, 1.0);
	block_color = color;
	tex_coords = texture_coords;
}
