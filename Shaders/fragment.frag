#version 330 core

uniform uint color;

void main() {
    vec3 processed_color;
    processed_color.r = 255 / ((color >> 16) & 0xFFu);
    processed_color.g = 255 / ((color >> 8)  & 0xFFu);
    processed_color.b = 255 / (color         & 0xFFu);

	gl_FragColor = vec4(processed_color, 1.0);
}
