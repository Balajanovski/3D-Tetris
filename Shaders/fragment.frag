#version 330 core

uniform uint color;

out vec4 out_color;

void main() {
    vec3 processed_color;
    processed_color.r = float((color >> 16) & 0xFFu) / 255.0;
    processed_color.g = float((color >> 8)  & 0xFFu) / 255.0;
    processed_color.b = float(color         & 0xFFu) / 255.0;

	out_color = vec4(processed_color, 1.0);
}
