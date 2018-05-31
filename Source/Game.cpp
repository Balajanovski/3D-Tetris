// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#include <set>
#include <algorithm>

#ifndef NDEBUG
#include <iostream>
#endif

Game::Game() :
        view_component("Shaders/vertex.vert",
                       "Shaders/fragment.frag",
                       "Resources/Textures/block.jpg",
                       "Resources/Fonts/Roboto-Regular.ttf"),
        input_queue(view_component.get_window()),

        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this),
        ghost_tetromino(current_tetromino)

{
    previous_tetromino_move_time = glfwGetTime();
}

void Game::begin() {
    static double limit_FPS = 1.0 / FPS;

    // Game loop
    double previous_time = glfwGetTime(), timer = previous_time;
    double delta_time = 0, now_time = 0;
    int frames       = 0, updates = 0;
    while (!close_game) {
        // Handle delta time
        now_time = glfwGetTime();
        delta_time += (now_time - previous_time) / limit_FPS;
        previous_time = now_time;

        // Poll events for controls
        glfwPollEvents();

        // If game is in game over state
        // game should not be updated
        if (!game_over) {
            // Update game at FPS
            while (delta_time >= 1.0) {
                if (!game_over) {
                    tick();
                }

                ++updates;
                --delta_time;
            }
        } else {
            while (delta_time >= 1.0) {
                window_control();

                ++updates;
                --delta_time;
            }
        }


        // Rendering
        // ---------

        // Draw tetrominos
        view_component.clear_screen();
        if (current_tetromino.get_state() != TetrominoUtil::TetrominoState::LANDED) {
            view_component.draw_tetromino(current_tetromino, false);

            // Draw ghost indicator tetromino
            ghost_tetromino = current_tetromino;
            ghost_tetromino.jump_down(true);
            view_component.draw_tetromino(ghost_tetromino, true);
        }
        for (auto& t : landed) {
            view_component.draw_tetromino(t, false);
        }

        // Draw the border
        view_component.draw_border();

        // Display text
        view_component.draw_message(glm::ivec2{10, SCREEN_HEIGHT - 40},
                                    0.65f, "Score: " + std::to_string(score));
        if (game_over) {
            view_component.draw_message(glm::ivec2{50, SCREEN_HEIGHT - (SCREEN_HEIGHT / 2) + 60},
                                        1.5f, "Game Over");
            view_component.draw_message(glm::ivec2{35, SCREEN_HEIGHT - (SCREEN_HEIGHT / 2)},
                                        0.6f, "Press Esc to Quit or R to Reset");
        }

        ++frames;

        view_component.swap_buffers();
    }
}

void Game::reset() {
    game_over = false;
    close_game = false;
    score = 0;

    landed.clear();
    current_tetromino = Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this);
    ghost_tetromino = current_tetromino;
    previous_tetromino_move_time = glfwGetTime();
}

void Game::tick() {
    if (current_tetromino.get_state() == TetrominoUtil::TetrominoState::LANDED) {
        current_tetromino =
                Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 6)), this);
    }

    // Handle input
    int input_key;
    do {
        input_key = window_control();
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
                current_tetromino.translate_down(false);

                // Reset move time
                previous_tetromino_move_time = glfwGetTime();
#ifndef NDEBUG
                std::cerr << "Input: Down\n";
#endif
                break;
            case GLFW_KEY_SPACE :
#ifndef NDEBUG
                std::cerr << "Input: Space\n";
#endif
                current_tetromino.jump_down(false);
                return; // Exit function, because last translate down is redundant
                break;  // Break statement is redundant, yet there for stylistic reasons
            case GLFW_KEY_ESCAPE :
                return; // Exit function, because quit command was invoked
                break;
            case GLFW_KEY_R :
                return;
                break;
            default:
                break;
        }
    } while(input_key != GLFW_KEY_UNKNOWN);

    // Handle game over
    for (auto& t : landed) {
        if (t.get_top_left_point().y <= 1) {
            game_over = true;
        }
    }

    // Move down current tetromino if time passed
    if ((glfwGetTime() - previous_tetromino_move_time) >= TIME_BETWEEN_TETROMINO_MOVEMENTS) {
        current_tetromino.translate_down(false);
        previous_tetromino_move_time = glfwGetTime();
    }

}

int Game::window_control() {
    int input_key = input_queue.fetch();
    switch(input_key) {
        case GLFW_KEY_ESCAPE :
#ifndef NDEBUG
            std::cerr << "Input: Escape\n";
#endif
            close_game = true;
            break;
        case GLFW_KEY_R :
#ifndef NDEBUG
            std::cerr << "Input: R (Reset)\n";
#endif
            reset();
            break;
    }

    if (view_component.should_window_close()) {
        close_game = true;
        return GLFW_KEY_ESCAPE; // Exit function, because quit command was invoked
    }

    return input_key;
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
    int highest_cleared_row = -1;
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

        // If row is full proceed to clear
        if (row_full) {
            ++rows_cleared;
            highest_cleared_row = y;
            for (int x = 0; x < GAME_WIDTH; ++x) {
                for (auto& l : landed) {
                    l.remove_block(glm::ivec2{x, y});
                }
            }

        }
    }

    if (highest_cleared_row != -1) {
        // Get all blocks above deleted row to move down
        std::vector<glm::ivec2> blocks_to_move_down;
        for (auto& l : landed) {
            auto blocks = l.get_blocks();
            for (auto& b : blocks) {
                if (b.y < highest_cleared_row) {
                    blocks_to_move_down.push_back(b);
                }
            }
            l.translate_blocks_down(blocks_to_move_down, rows_cleared);
            blocks_to_move_down.clear();
        }
    }

    // Lookup table which matches the rows cleared with the score to be awarded
    static const int ROWS_CLEARED_SCORING_TABLE[4] = {
            40,
            100,
            300,
            1200,
    };
    if (rows_cleared >= 1 && rows_cleared <= 4) {
        score += ROWS_CLEARED_SCORING_TABLE[rows_cleared - 1];
    }

    // Find and delete all tetrominos with no blocks
    std::remove_if(landed.begin(), landed.end(), [](const Tetromino& t) -> bool {
        return (t.num_blocks() == 0);
    });

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