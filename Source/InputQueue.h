// 
// Created by Balajanovski on 28/04/2018.
//

#ifndef INC_3D_TETRIS_INPUTQUEUE_H
#define INC_3D_TETRIS_INPUTQUEUE_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <deque>
#include <memory>

class InputQueue {
    using GLFWkey = int;
public:
    // Set callback as friend
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    explicit InputQueue(GLFWwindow* win);

    GLFWkey fetch();
private:
    static constexpr int MAX_ELEMENTS_IN_QUEUE = 20;

    std::deque<GLFWkey> queue;
    GLFWwindow* window;
};


#endif //INC_3D_TETRIS_INPUTQUEUE_H
