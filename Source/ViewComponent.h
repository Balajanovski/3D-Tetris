// 
// Created by Balajanovski on 24/04/2018.
//

#ifndef INC_3D_TETRIS_VIEWCOMPONENT_H
#define INC_3D_TETRIS_VIEWCOMPONENT_H

#include "Util/Shader.h"
#include "FontComponent.h"
#include "Constants.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

class Tetromino;

class ViewComponent {
public:
    ViewComponent(const std::string& vert_shader_src,
                  const std::string& frag_shader_src,
                  const std::string& texture_src,
                  const std::string& font_src);
    ~ViewComponent();

    void draw_tetromino(const Tetromino &tetromino, bool is_ghost_tetromino);
    void draw_border();
    void draw_message(glm::ivec2 top_left, float scale, const std::string& msg);

    void swap_buffers();
    void clear_screen();

    void rotate_view_left()  { view_rot -= ROTATION_INCREMENT;   }
    void rotate_view_right() { view_rot += ROTATION_INCREMENT;   }
    void reset_view_rotation()        { view_rot = 0.0f; }

    // Getters
    GLFWwindow* get_window() { return window; }
    bool should_window_close() { return glfwWindowShouldClose(window); }
private:
    std::shared_ptr<FontComponent> font_component;

    std::unique_ptr<Shader> shader_prog;
    GLFWwindow* window;

    // Used for rendering objects
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    GLuint block_texture;

    glm::mat4 model_matrix;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    // Value used to determine how much the view matrix should be rotated
    float view_rot = 0.0f;

    void draw_block(const glm::ivec2 &block, uint32_t color, bool is_faded);
    static constexpr int FADING_FACTOR = 50; // Expressed As Percentage

    // Is this the first time ViewComponent has drawn?
    // Used to determine whether to use glBufferData or glBufferSubData
    bool first_iteration = true;

    // Draw mode view component is in
    enum DrawMode {
        GRAPHICS_MODE = 0,
        TEXT_MODE = 1,
        NONE = 2,
    };
    DrawMode draw_mode = NONE;
};


#endif //INC_3D_TETRIS_VIEWCOMPONENT_H
