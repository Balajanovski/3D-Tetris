// 
// Created by Balajanovski on 24/04/2018.
//

#include "ViewComponent.h"
#include "Constants.h"
#include "Tetromino.h"

#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cassert>
#include <memory>
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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Clear GL color buffer
    glClearColor(0, 0, 0, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 5 * sizeof(int), (void*)0);
    glEnableVertexAttribArray(0);

    // Set vertex attribute for texture coords
    glVertexAttribPointer(1, 2, GL_INT, GL_FALSE, 5 * sizeof(int), (void*)(3 * sizeof(int)));
    glEnableVertexAttribArray(1);

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
    if (data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::runtime_error(std::string("fatal error: Failed to load texture\nTexture path: ") + texture_src);
    }
    stbi_image_free(data);

    // Prepare to go 3D
    // ----------------
    // Generate matrices
    // Set model matrix to translate from game to world space
    model_matrix = glm::translate(model_matrix, glm::vec3(-1.0f, 1.0f, -1.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(1.0f/SCREEN_WIDTH, 1.0/SCREEN_HEIGHT, 1.0f/SCREEN_WIDTH));
    model_matrix = glm::scale(model_matrix, glm::vec3(BLOCK_SIZE));
    model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, -1.0f, 1.0f));

    view_matrix  = glm::lookAt(glm::vec3(0.0f, -0.38f, 2.6f),
                               glm::vec3(0.0f, -0.38f, 0.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));
    projection_matrix = glm::perspective(glm::radians(50.0f),
                                         static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT),
                                         0.1f,
                                         100.0f);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::shared_ptr<std::array<int, 80>> generate_block_vertex_data(const glm::ivec2 &block);

void ViewComponent::draw_block(const glm::ivec2 &block, uint32_t color, bool is_faded) {
    auto vertices = generate_block_vertex_data(block);

    static unsigned int indices[] = {
            // Front face
            0, 1, 3,
            3, 2, 0,

            // Right-side face
            1, 4, 6,
            6, 3, 1,

            // Back face
            4, 5, 7,
            7, 6, 4,

            // Left-side face
            5, 0, 2,
            2, 7, 5,

            // Top face
            8, 9, 10,
            10, 11, 8,

            // Bottom face
            13, 12, 15,
            15, 14, 13,
    };

    shader_prog->use();

    // If it is the first iteration, glBufferData must be used to generate the buffers
    // Otherwise, use glBufferSubData to avoid reallocating the buffer every tick
    if (first_iteration) {
        // Send constants to shader
        shader_prog->set_int("SCREEN_WIDTH", SCREEN_WIDTH);
        shader_prog->set_int("SCREEN_HEIGHT", SCREEN_HEIGHT);
        shader_prog->set_int("BLOCK_SIZE", BLOCK_SIZE);

        // Send block texture to shader
        shader_prog->set_int("block_texture", 0);

        // Send matrices to shaders
        int model_loc = glGetUniformLocation(static_cast<GLuint>(shader_prog->ID()), "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

        int view_loc = glGetUniformLocation(static_cast<GLuint>(shader_prog->ID()), "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

        int projection_loc = glGetUniformLocation(static_cast<GLuint>(shader_prog->ID()), "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


        glBindVertexArray(vao);

        // Buffer data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(int), vertices.get(), GL_DYNAMIC_DRAW);

        // Buffer data into ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    } else {
        glBindVertexArray(vao);

        // Reset vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->size() * sizeof(int), vertices.get());

        // Ebo is not reset since it is unchanging

        glBindVertexArray(0);
    }

    // Set colour and supply it to shader
    glm::vec4 color_vec;
    if (is_faded) {
        color_vec = {static_cast<int>((color >> 16) & 0xFFu),
                     static_cast<int>((color >> 8) & 0xFFu),
                     static_cast<int>(color & 0xFFu),
                     FADING_FACTOR};
    } else {
        color_vec = {static_cast<int>((color >> 16) & 0xFFu),
                     static_cast<int>((color >> 8) & 0xFFu),
                     static_cast<int>(color & 0xFFu),
                     100};
    }
    int color_loc = glGetUniformLocation(static_cast<GLuint>(shader_prog->ID()), "block_color");
    glUniform4fv(color_loc, 1, glm::value_ptr(color_vec));

    // Draw block
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, block_texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // Since it is no longer the first iteration
    if (first_iteration) {
        first_iteration = false;
    }
}

void ViewComponent::draw_border() {
    static constexpr uint32_t BORDER_COLOR = 0xC0C0C0;

    // Draw bottom and top
    for (int x = -1; x < static_cast<int>(GAME_WIDTH + 1); ++x) {
        // Draw bottom
        draw_block(glm::ivec2{x, GAME_HEIGHT}, BORDER_COLOR, false);

        // Draw top
        draw_block(glm::ivec2{x, -1}, BORDER_COLOR, false);
    }

    // Draw sides
    for (int y = -1; y < static_cast<int>(GAME_HEIGHT + 1); ++y) {
        // Draw left
        draw_block(glm::ivec2{-1, y}, BORDER_COLOR, false);

        // Draw right
        draw_block(glm::ivec2{GAME_WIDTH, y}, BORDER_COLOR, false);
    }
}

std::shared_ptr<std::array<int, 80>> generate_block_vertex_data(const glm::ivec2 &block) {

    auto vertices = std::make_shared<std::array<int, 80>>(std::array<int, 80>{
        // Vertex coords        |Tex coords
        // x       y          z |
        block.x  , block.y  , 0, 0, 1, // Front top-left
        block.x+1, block.y  , 0, 1, 1, // Front top-right
        block.x  , block.y+1, 0, 0, 0, // Front bottom-left
        block.x+1, block.y+1, 0, 1, 0, // Front bottom-right

        block.x+1, block.y  , 1, 0, 1, // Back top-left
        block.x  , block.y  , 1, 1, 1, // Back top-right
        block.x+1, block.y+1, 1, 0, 0, // Back bottom-left
        block.x  , block.y+1, 1, 1, 0, // Back bottom-right

                                       // Separate vertices for top & bottom
                                       // Due to texture coords
        block.x  , block.y  , 0, 0, 0, // Front top-left (drawn for top face)
        block.x  , block.y  , 1, 0, 1, // Back top-right (drawn for top face)
        block.x+1, block.y  , 1, 1, 1, // Back top-left  (drawn for top face)
        block.x+1, block.y  , 0, 1, 0, // Front top-right (drawn for top face)

        block.x+1, block.y+1, 0, 1, 1, // Front bottom-right (drawn for bottom face)
        block.x  , block.y+1, 0, 0, 1, // Front bottom-left (drawn for bottom face)
        block.x  , block.y+1, 1, 0, 0, // Back bottom-right  (drawn for bottom face)
        block.x+1, block.y+1, 1, 1, 0, // Back bottom-left (drawn for bottom face)
    });


    return vertices;
};

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

