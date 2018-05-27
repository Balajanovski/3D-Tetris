#version 330 core

in vec4 block_color;

out vec4 out_color;

void main() {
    vec3 processed_color;
    processed_color.r = float(block_color.r) / 255.0;
    processed_color.g = float(block_color.g) / 255.0;
    processed_color.b = float(block_color.b) / 255.0;

	out_color = vec4(processed_color, float(block_color.a) / 100.0);
}
