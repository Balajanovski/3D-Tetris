// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#include <set>

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
                Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 6)), this);
    }

    // Handle input
    int input_key;
    do {
        input_key = input_queue.fetch();
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
    } while(input_key != GLFW_KEY_UNKNOWN);


    // Handle quit event
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

void Game::handle_row_clearing() {
    // Get estimate for how high to check
    int row_clearing_cap = GAME_HEIGHT;
    for (auto &t : landed) {
        int top_left_y = t.get_top_left_point().y;

        if (top_left_y < row_clearing_cap) {
            row_clearing_cap = top_left_y;
        }
    }

    int rows_cleared = 0;
    for (int y = GAME_HEIGHT - 1; y >= row_clearing_cap; --y) {
        // Determine if row is full
        bool row_full = true;
        for (int x = 0; x < GAME_WIDTH; ++x) {
            bool found_block = false;
            for (auto& l : landed) {
                if (l.block_exists(glm::ivec2{x, y})) {
                    found_block = true;
                }
            }
            if (!found_block) {
                row_full = false;
            }
        }
        printf("\n");

        // If row is full proceed to clear
        if (row_full) {
            ++rows_cleared;
            for (int x = 0; x < GAME_WIDTH; ++x) {
                for (auto& l : landed) {
                    l.remove_block(glm::ivec2{x, y});
                }
            }

            // Get all blocks above deleted row to move down
            for (auto& l : landed) {
                auto blocks = l.get_blocks();
                for (auto& b : blocks) {
                    if (b.y < y) {
                        l.translate_block_down(b);
                    }
                }
            }


        }
    }

    // Find and delete all tetrominos with no blocks
    /*std::set<int> indexes_of_tetrominos_with_no_blocks;
    for (int i = 0; i < landed.size(); ++i) {
        if (landed[i].num_blocks() <= 0) {
            indexes_of_tetrominos_with_no_blocks.insert(i);
        }
    }
    for (auto& index : indexes_of_tetrominos_with_no_blocks) {
        landed.erase(landed.begin() + index);
    }*/

}

void Game::add_landed(const Tetromino& tetromino) {
    landed.push_back(tetromino);

    if (tetromino.highest_block() <= 1) {
        game_over = true;
    }

    handle_row_clearing();
}

bool Game::check_collision(const Tetromino& new_pos) const {
    for (auto iter = landed.cbegin(); iter != landed.cend(); ++iter) {
        if (iter->tetromino_overlaps(new_pos)) {
            return true;
        }
    }

    return false;
}