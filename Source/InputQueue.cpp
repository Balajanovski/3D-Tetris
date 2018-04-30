// 
// Created by Balajanovski on 28/04/2018.
//

#include "InputQueue.h"

#include <algorithm>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == 0) {
        return;
    }

    auto input_queue = static_cast<InputQueue*>(glfwGetWindowUserPointer(window));

    if (input_queue->queue.size() >= InputQueue::MAX_ELEMENTS_IN_QUEUE - 1) {
        input_queue->queue.pop_back();
    }

    input_queue->queue.push_front(key);
}

InputQueue::InputQueue(GLFWwindow* win) : window(win) {
    // Use user pointer to allow access to class within callback
    glfwSetWindowUserPointer(window, this);

    // Set callback
    glfwSetKeyCallback(window, key_callback);

    // Reset user pointer so no dangling pointers are accessed
    //glfwSetWindowUserPointer(window, window);
}

InputQueue::GLFWkey InputQueue::fetch() {
    if (queue.empty()) {
        return GLFW_KEY_UNKNOWN;
    } else {
        GLFWkey back = queue.back();
        queue.pop_back();
        return back;
    }
}
