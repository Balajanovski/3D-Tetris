#version 330 core

layout (location = 0) in ivec2 pos;

uniform int GAME_WIDTH;
uniform int GAME_HEIGHT;
uniform int BLOCK_SIZE;

void main() {
    vec2 processed_pos;
    processed_pos.x = float(pos.x * BLOCK_SIZE) / float(GAME_WIDTH) * 2.0 - 1.0;
    processed_pos.y = float(pos.y * BLOCK_SIZE) / float(GAME_HEIGHT) * 2.0 - 1.0;

	gl_Position = vec4(processed_pos, 0.0, 1.0);
}
