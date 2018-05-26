// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#ifndef NDEBUG
#include <iostream>
#endif

Game::Game() :
        view_component("Shaders/vertex.vert", "Shaders/fragment.frag"),
        input_queue(view_component.get_window()),

        // Initialise current tetromino with garbage for it has no default constructor
        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this)

{
    previous_tetromino_move_time = glfwGetTime();
}

void Game::begin() {
    static double limit_FPS = 1.0 / FPS;

    // Game loop
    double previous_time = glfwGetTime(), timer = previous_time;
    double delta_time = 0, now_time = 0;
    int frames       = 0, updates = 0;
    while (!game_over) {
        // Handle delta time
        now_time = glfwGetTime();
        delta_time += (now_time - previous_time) / limit_FPS;
        previous_time = now_time;

        // Poll events for controls
        glfwPollEvents();

        // Update game at FPS
        while (delta_time >= 1.0) {
            if (!game_over) {
                tick();
            }

            ++updates;
            --delta_time;
        }

        // Render code code
        view_component.clear_screen();
        if (current_tetromino.get_state() != TetrominoUtil::TetrominoState::LANDED) {
            view_component.draw_tetromino(current_tetromino);
        }

        for (auto& t : landed) {
            view_component.draw_tetromino(t);
        }
        ++frames;

        view_component.swap_buffers();
    }
}

void Game::tick() {
    if (current_tetromino.get_state() == TetrominoUtil::TetrominoState::LANDED) {
        current_tetromino =
                Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this);
    }

    // Handle input
    int input_key = input_queue.fetch();
    switch (input_key) {
        case GLFW_KEY_LEFT :
            current_tetromino.translate_left();
#ifndef NDEBUG
            std::cerr << "Input: Left\n";
#endif
            break;
        case GLFW_KEY_RIGHT :
            current_tetromino.translate_right();
#ifndef NDEBUG
            std::cerr << "Input: Right\n";
#endif
            break;
        case GLFW_KEY_UP :
            current_tetromino.rotate_right();
#ifndef NDEBUG
            std::cerr << "Input: Up\n";
#endif
            break;
        case GLFW_KEY_DOWN :
            current_tetromino.rotate_left();
#ifndef NDEBUG
            std::cerr << "Input: Down\n";
#endif
            break;
        case GLFW_KEY_SPACE :
#ifndef NDEBUG
            std::cerr << "Input: Space\n";
#endif
            current_tetromino.jump_down();
            return; // Exit function, because last translate down is redundant
            break;  // Break statement is redundant, yet there for stylistic reasons
        case GLFW_KEY_ESCAPE :
#ifndef NDEBUG
            std::cerr << "Input: Escape\n";
#endif
            game_over = true;
            return; // Exit function, because quit command was invoked
            break;
        default:
            break;
    }

    if (view_component.should_window_close()) {
        game_over = true;
        return; // Exit function, because quit command was invoked
    }

    // Handle game over
    for (auto& t : landed) {
        if (t.get_top_left_point().y <= 1) {
            game_over = true;
        }
    }

    // Move down current tetromino if time passed
    if ((glfwGetTime() - previous_tetromino_move_time) >= TIME_BETWEEN_TETROMINO_MOVEMENTS) {
        current_tetromino.translate_down();
        previous_tetromino_move_time = glfwGetTime();
    }

}

void Game::add_landed(const Tetromino& t) {
    landed.push_back(t);

    if (t.highest_block() <= 1) {
        game_over = true;
    }
}

bool Game::check_collision(const Tetromino& new_pos) const {
    for (auto iter = landed.cbegin(); iter != landed.cend(); ++iter) {
        if (iter->tetromino_overlaps(new_pos)) {
            return true;
        }
    }

    return false;
}