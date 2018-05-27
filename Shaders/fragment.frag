#version 330 core

in vec4 block_color;
in vec2 tex_coords;

out vec4 out_color;

uniform sampler2D block_texture;

void main() {
    vec3 processed_color;
    processed_color.r = float(block_color.r) * texture(block_texture, tex_coords).r;
    processed_color.g = float(block_color.g) * texture(block_texture, tex_coords).r;
    processed_color.b = float(block_color.b) * texture(block_texture, tex_coords).r;

    // This clamp with desaturation algorithm was taken from Bisqwit with his permission
    // The original source can be found via the link below:
    // https://gist.github.com/bisqwit/6fa30964eacefeea2954e5c42c966114
    float l = dot(processed_color, vec3(0.29900,  0.58700,  0.11400));
    if (l > 255.0f) {
        processed_color = vec3(1.0f, 1.0f, 1.0f);
    } else if (l <= 0.0f) {
        processed_color = vec3(0.0f, 0.0f, 0.0f);
    } else {
        float s = 1.0f;
        for (int n = 0; n < 3; ++n) {
            if (processed_color[n] > 255.0f) {
                s = min(s, (l - 255.0f) / (l - processed_color[n]));
            } else if (processed_color[n] < 0.0f) {
                s = min(s, l / (l - processed_color[n]));
            }
        }

        if (s != 1.0f) {
            for (int i = 0; i < 3; ++i) {
                processed_color[i] = (processed_color[i] - l) * s + l;
            }
        }
    }

    processed_color.r = processed_color.r / 255.0f;
    processed_color.g = processed_color.g / 255.0f;
    processed_color.b = processed_color.b / 255.0f;

	out_color = vec4(processed_color, float(block_color.a) / 100.0);
}
