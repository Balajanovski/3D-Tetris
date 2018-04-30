#version 330 core

uniform uint color;

out vec4 out_color;

void main() {
    vec3 processed_color;
    processed_color.r = 255.0 / float((color >> 16) & 0xFFu);
    processed_color.g = 255.0 / float((color >> 8)  & 0xFFu);
    processed_color.b = 255.0 / float(color         & 0xFFu);

	out_color = vec4(processed_color, 1.0);
}
