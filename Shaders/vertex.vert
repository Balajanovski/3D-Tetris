#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in uint color;

uniform uint GAME_WIDTH;
uniform uint GAME_HEIGHT;
uniform uint BLOCK_SIZE;

void main() {
    vec2 processed_pos;
    processed_pos.x = (pos.x * BLOCK_SIZE) / float(GAME_WIDTH) * 2.0f - 1.0f;
    processed_pos.y = (pos.y * BLOCK_SIZE) / float(GAME_HEIGHT) * 2.0f - 1.0f;

	gl_Position = vec4(processed_pos, 0.0f, 1.0);
}
