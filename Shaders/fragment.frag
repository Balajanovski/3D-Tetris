#version 330 core

in vec4 block_color;
in vec2 tex_coords;

out vec4 out_color;

uniform sampler2D block_texture;

void main() {
    vec4 processed_color;
    processed_color.r = float(block_color.r) / 255.0;
    processed_color.g = float(block_color.g) / 255.0;
    processed_color.b = float(block_color.b) / 255.0;
    processed_color.a = 1.0;

    // Lerp texture onto colour
    processed_color = mix(processed_color, texture(block_texture, tex_coords), 0.7);
    processed_color.a = float(block_color.a) / 100.0;

	out_color = processed_color;
}
