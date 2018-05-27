// 
// Created by Balajanovski on 24/04/2018.
//

#include "ViewComponent.h"
#include "Constants.h"
#include "Tetromino.h"

#include <stb_image/stb_image.h>

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

ViewComponent::ViewComponent(const std::string& vert_shader_src,
                             const std::string& frag_shader_src,
                             const std::string& texture_src) {
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
    glClear(GL_COLOR_BUFFER_BIT);

    // Create shader
    shader_prog.reset(new Shader(vert_shader_src.c_str(), frag_shader_src.c_str()));

    // Generate vertex array object
    glGenVertexArrays(1, &vao);

    // Generate vertex buffer object
    glGenBuffers(1, &vbo);

    // Generate element buffer object
    glGenBuffers(1, &ebo);

    // Bind vao to store VertexAttribPointer call
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Setting vertex attributes
    // -------------------------
    // Set vertex attribute for position
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 8 * sizeof(int), (void*)0);
    glEnableVertexAttribArray(0);

    // Set vertex attribute for color
    glVertexAttribPointer(1, 4, GL_INT, GL_FALSE, 8 * sizeof(int), (void*)(2 * sizeof(int)));
    glEnableVertexAttribArray(1);

    // Set vertex attribute for texture coords
    glVertexAttribPointer(2, 2, GL_INT, GL_FALSE, 8 * sizeof(int), (void*)(6 * sizeof(int)));
    glEnableVertexAttribArray(2);

    // Unbind vao (vertex array object)
    glBindVertexArray(0);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load and create texture
    // -----------------------
    glGenTextures(1, &block_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, block_texture);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texture_src.c_str(), &width, &height, &nr_channels, 0);
    printf("w: %d. h: %d. channels: %d.\n", width, height, nr_channels);
    if (data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::runtime_error(std::string("fatal error: Failed to load texture\nTexture path: ") + texture_src);
    }
    stbi_image_free(data);

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
    std::array<int, 32> vertices;
    if (is_faded) {
        vertices = {
                // Top-left
                // Block coords       Colours and opacity
                block.x,   block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      FADING_FACTOR, // Set opacity to faded
                // Texture coords
                0, 1,

                // Top-right
                // Block coords       Colours and opacity
                block.x+1, block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      FADING_FACTOR, // Set opacity to faded
                // Texture coords
                1, 1,

                // Bottom-left
                // Block coords       Colours and opacity
                block.x,   block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      FADING_FACTOR, // Set opacity to faded
                // Texture coords
                0, 0,

                // Bottom-right
                // Block coords       Colours and opacity
                block.x+1, block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      FADING_FACTOR, // Set opacity to faded
                // Texture coords
                1, 0,
        };
    } else {
        vertices = {
                // Top-left
                // Block coords       Colours and opacity
                block.x,   block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      100, // Set opacity to full since not faded
                // Texture coords
                0, 1,

                // Top-right
                // Block coords       Colours and opacity
                block.x+1, block.y,   static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      100, // Set opacity to full since not faded
                // Texture coords
                1, 1,

                // Bottom-left
                // Block coords       Colours and opacity
                block.x,   block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),
                                      100, // Set opacity to full since not faded
                // Texture coords
                0, 0,

                // Bottom-right
                // Block coords       Colours and opacity
                block.x+1, block.y+1, static_cast<int>((color >> 16) & 0xFFu),
                                      static_cast<int>((color >> 8) & 0xFFu),
                                      static_cast<int>(color & 0xFFu),          // Bottom-right
                                      100, // Set opacity to full since not faded
                // Texture coords
                1, 0,
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
        shader_prog->set_int("block_texture", 0);

        glBindVertexArray(vao);

        // Buffer data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (vertices).size() * sizeof(int), &vertices[0], GL_DYNAMIC_DRAW);

        // Buffer data into ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    } else {
        glBindVertexArray(vao);

        // Reset vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (vertices).size() * sizeof(int), &vertices[0]);

        // Ebo is not reset since it is unchanging

        glBindVertexArray(0);
    }

    // Draw block
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, block_texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

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