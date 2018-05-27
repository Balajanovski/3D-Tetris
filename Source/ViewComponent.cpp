// 
// Created by Balajanovski on 24/04/2018.
//

#include "ViewComponent.h"
#include "Constants.h"
#include "Tetromino.h"

#include <array>
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

inline void init_glfw();
inline void init_glad();
#ifndef NDEBUG
static void glfw_error(int id, const char* description);
#endif

ViewComponent::ViewComponent(const std::string& vert_shader_src, const std::string& frag_shader_src) {
#ifndef NDEBUG
    glfwSetErrorCallback(&glfw_error);
#endif
    init_glfw();

    // Create the window
    window = (glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris", NULL, NULL));
    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("fatal error: Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    init_glad();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow* window, int width, int height) {
                                       glViewport(0, 0, width, height);
                                   }
    );

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clear GL color buffer
    glClearColor(0, 0, 0, 0.0f);
    assert(glGetError() == GL_NO_ERROR);

    glClear(GL_COLOR_BUFFER_BIT);
    assert(glGetError() == GL_NO_ERROR);

    // Create shader
    shader_prog.reset(new Shader(vert_shader_src.c_str(), frag_shader_src.c_str()));

    // Generate vertex array object
    glGenVertexArrays(1, &vao);
    assert(glGetError() == GL_NO_ERROR);

    // Generate vertex buffer object
    glGenBuffers(1, &vbo);
    assert(glGetError() == GL_NO_ERROR);

    // Generate element buffer object
    glGenBuffers(1, &ebo);
    assert(glGetError() == GL_NO_ERROR);

    // Bind vao to store VertexAttribPointer call
    glBindVertexArray(vao);
    assert(glGetError() == GL_NO_ERROR);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    assert(glGetError() == GL_NO_ERROR);

    // Set vertex attribute for position
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 6 * sizeof(int), (void*)0);
    assert(glGetError() == GL_NO_ERROR);
    glEnableVertexAttribArray(0);
    assert(glGetError() == GL_NO_ERROR);

    // Set vertex attribute for color
    glVertexAttribPointer(1, 4, GL_INT, GL_FALSE, 6 * sizeof(int), (void*)(2 * sizeof(int)));
    assert(glGetError() == GL_NO_ERROR);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NO_ERROR);

    // Unbind vao (vertex array object)
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);
}

ViewComponent::~ViewComponent() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void ViewComponent::draw_tetromino(const Tetromino &tetromino, bool is_ghost_tetromino) {
    uint32_t color = tetromino.get_color();
    for (const auto& block : tetromino.get_blocks()) {
        draw_block(block, color, is_ghost_tetromino);
    }
}

void ViewComponent::swap_buffers() {
    glfwSwapBuffers(window);
}

void ViewComponent::clear_screen() {
    glClearColor(0, 0, 0, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ViewComponent::draw_block(const glm::ivec2 &block, uint32_t color, bool is_faded) {
    std::array<int, 24> vertices;
    if (is_faded) {
        vertices = {
                block.x,   block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Top-left
                                      FADING_FACTOR, // Set opacity to faded
                block.x+1, block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Top-right
                                      FADING_FACTOR, // Set opacity to faded
                block.x,   block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Bottom-left
                                      FADING_FACTOR, // Set opacity to faded
                block.x+1, block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Bottom-right
                                      FADING_FACTOR, // Set opacity to faded
        };
    } else {
        vertices = {
                block.x,   block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Top-left
                                      100, // Set opacity to full since not faded
                block.x+1, block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Top-right
                                      100, // Set opacity to full since not faded
                block.x,   block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Bottom-left
                                      100, // Set opacity to full since not faded
                block.x+1, block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Bottom-right
                                      100, // Set opacity to full since not faded
        };
    }


    static unsigned int indices[] = {
            1, 3, 0,
            3, 2, 0
    };

    shader_prog->use();

    // If it is the first iteration, glBufferData must be used to generate the buffers
    // Otherwise, use glBufferSubData to avoid reallocating the buffer every tick
    if (first_iteration) {
        // Supply constants to shader
        shader_prog->set_int("SCREEN_WIDTH", SCREEN_WIDTH);
        shader_prog->set_int("SCREEN_HEIGHT", SCREEN_HEIGHT);
        shader_prog->set_int("BLOCK_SIZE", BLOCK_SIZE);

        glBindVertexArray(vao);

        // Buffer data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferData(GL_ARRAY_BUFFER, (vertices).size() * sizeof(int), &vertices[0], GL_DYNAMIC_DRAW);
        assert(glGetError() == GL_NO_ERROR);

        // Buffer data into ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        assert(glGetError() == GL_NO_ERROR);

        glBindVertexArray(0);
    } else {
        glBindVertexArray(vao);

        // Reset vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        assert(glGetError() == GL_NO_ERROR);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (vertices).size() * sizeof(int), &vertices[0]);
        assert(glGetError() == GL_NO_ERROR);

        // Ebo is not reset since it is unchanging

        glBindVertexArray(0);
    }

    // Draw block
    glBindVertexArray(vao);
    assert(glGetError() == GL_NO_ERROR);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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

#ifndef NDEBUG
static void glfw_error(int id, const char* description) {
    std::cerr << description << "\n";
}
#endif