// 
// Created by Balajanovski on 24/04/2018.
//

#include "ViewComponent.h"
#include "Constants.h"
#include "Tetromino.h"

#include <cassert>

inline void init_glfw();
inline void init_glad();

ViewComponent::ViewComponent(const std::string &vert_shader_src, const std::string &frag_shader_src) {
    init_glfw();

    // Create the window
    window = (glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris", NULL, NULL));
    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("fatal error: Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    init_glad();

    // Create shader
    shader_prog.reset(new Shader(vert_shader_src.c_str(), frag_shader_src.c_str()));
    shader_prog->use();

    // Supply constants to shader
    shader_prog->set_unsigned_int("GAME_WIDTH", GAME_WIDTH);
    shader_prog->set_unsigned_int("GAME_HEIGHT", GAME_HEIGHT);
    shader_prog->set_unsigned_int("BLOCK_SIZE", BLOCK_SIZE);

    // Generate vertex buffer object
    glGenBuffers(1, &vbo);
    assert(glGetError() == GL_NO_ERROR);

    // Generate element buffer object
    glGenBuffers(1, &ebo);
    assert(glGetError() == GL_NO_ERROR);

    // Generate vertex array object
    glGenVertexArrays(1, &vao);
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(vao);
    assert(glGetError() == GL_NO_ERROR);

    // Bind vbo (vertex buffer object)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    assert(glGetError() == GL_NO_ERROR);

    // Bind ebo (element buffer object)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    assert(glGetError() == GL_NO_ERROR);

    // Unbind vao (vertex array object)
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ViewComponent::~ViewComponent() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void ViewComponent::draw_tetromino(const Tetromino &tetromino) {
    uint32_t color = tetromino.get_color();
    for (const auto& block : tetromino.get_blocks()) {
        draw_block(block, color);
    }
}

void ViewComponent::draw_block(const glm::ivec2 &block, uint32_t color) {
    int vertices[] = {
            block.x, block.y,      // Top-left
            block.x+1, block.y,    // Top-right
            block.x, block.y+1,    // Bottom-left
            block.x+1, block.y+1,  // Bottom-right
    };

    static unsigned int indices[] = {
            1, 3, 0,
            3, 2, 0
    };

    // If it is the first iteration, glBufferData must be used to generate the buffers
    // Otherwise, use glBufferSubData to avoid reallocating the buffer every tick
    if (first_iteration) {
        // Buffer data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        assert(glGetError() == GL_NO_ERROR);

        // Buffer data into ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        assert(glGetError() == GL_NO_ERROR);
    } else {
        // Reset vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        assert(glGetError() == GL_NO_ERROR);

        // Ebo is not reset since it is unchanging
    }

    shader_prog->set_unsigned_int("color", color);
    shader_prog->use();

    // Draw block
    glBindVertexArray(vao);
    assert(glGetError() == GL_NO_ERROR);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);

    // Since it is no longer the first iteration
    if (first_iteration) {
        first_iteration = false;
    }
}

inline void init_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

inline void init_glad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}