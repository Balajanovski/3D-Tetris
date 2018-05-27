// 
// Created by Balajanovski on 24/04/2018.
//

#ifndef INC_3D_TETRIS_VIEWCOMPONENT_H
#define INC_3D_TETRIS_VIEWCOMPONENT_H

#include "Util/Shader.h"
#include "RandomNumberComponent.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <glm/vec2.hpp>

class Tetromino;

class ViewComponent {
public:
    ViewComponent(const std::string& vert_shader_src, const std::string& frag_shader_src);
    ~ViewComponent();

    void draw_tetromino(const Tetromino &tetromino, bool is_ghost_tetromino);
    void swap_buffers();
    void clear_screen();

    // Getters
    GLFWwindow* get_window() { return window; }
    bool should_window_close() { return glfwWindowShouldClose(window); }
private:
    std::unique_ptr<Shader> shader_prog;
    GLFWwindow* window;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    void draw_block(const glm::ivec2 &block, uint32_t color, bool is_faded);
    static constexpr int FADING_FACTOR = 50; // Expressed As Percentage

    // Is this the first time ViewComponent has drawn?
    // Used to determine whether to use glBufferData or glBufferSubData
    bool first_iteration = true;
};


#endif //INC_3D_TETRIS_VIEWCOMPONENT_H
