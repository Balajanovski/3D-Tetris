// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#include "Util/Filesystem.h"

#include <set>
#include <vector>
#include <algorithm>

#ifndef NDEBUG
#include <iostream>
#endif

Game::Game() :
        view_component(FileSystem::getPath("Shaders/vertex.vert"),
                       FileSystem::getPath("Shaders/fragment.frag"),
                       FileSystem::getPath("Resources/Textures/block.jpg"),
                       FileSystem::getPath("Resources/Fonts/Roboto-Regular.ttf")),
        input_queue(view_component.get_window()),
        sound_component(std::vector<std::string>{FileSystem::getPath("Resources/Music/ThemeA.wav"),
                                                 FileSystem::getPath("Resources/Music/ThemeB.wav"),
                                                 FileSystem::getPath("Resources/Music/ThemeC.wav"),
                                                 FileSystem::getPath("Resources/Music/ThemeD.wav")},
                        FileSystem::getPath("Resources/Music/Gameover.wav"),
                        std::map<SoundUtil::SFXSound, std::string>{
                                {SoundUtil::SFXSound::TETRIS, FileSystem::getPath("Resources/SFX/Tetris.wav")},
                                {SoundUtil::SFXSound::LINE_CLEAR, FileSystem::getPath("Resources/SFX/LineClear.wav")},
                                {SoundUtil::SFXSound::MOVE, FileSystem::getPath("Resources/SFX/Move.wav")},
                        },
                        this),

        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 6)), this),
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

        // If game is in game over state or paused
        // game should not be updated
        if (!game_over && !paused) {
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

        // Rotate scene
        if (game_over) {
            view_component.rotate_view_right();
        } else if (paused) {
            view_component.rotate_view_left();
        } else {
            view_component.reset_view_rotation();
        }

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
        } else if (paused) {
            view_component.draw_message(glm::ivec2{35, SCREEN_HEIGHT - (SCREEN_HEIGHT / 2) + 60},
                                        1.3f, "Game Paused");
            view_component.draw_message(glm::ivec2{105, SCREEN_HEIGHT - (SCREEN_HEIGHT / 2)},
                                        0.6f, "Press P to unpause");
        }

        ++frames;

        view_component.swap_buffers();

        // Play music
        if (!game_over) {
            sound_component.play_music();
        } else {
            sound_component.play_game_over_music();
        }
    }
}

void Game::reset() {
    game_over = false;
    close_game = false;
    paused = false;
    score = 0;

    landed.clear();
    current_tetromino = Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 6)), this);
    ghost_tetromino = current_tetromino;
    previous_tetromino_move_time = glfwGetTime();

    view_component.reset_view_rotation();
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
                sound_component.play_sfx(SoundUtil::SFXSound::MOVE);
#ifndef NDEBUG
                std::cerr << "Input: Left\n";
#endif
                break;
            case GLFW_KEY_RIGHT :
                current_tetromino.translate_right();
                sound_component.play_sfx(SoundUtil::SFXSound::MOVE);
#ifndef NDEBUG
                std::cerr << "Input: Right\n";
#endif
                break;
            case GLFW_KEY_UP :
                current_tetromino.rotate_right();
                sound_component.play_sfx(SoundUtil::SFXSound::MOVE);
#ifndef NDEBUG
                std::cerr << "Input: Up\n";
#endif
                break;
            case GLFW_KEY_DOWN :
                current_tetromino.translate_down(false);
                sound_component.play_sfx(SoundUtil::SFXSound::MOVE);

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
                sound_component.play_sfx(SoundUtil::SFXSound::MOVE);
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
    auto input_key = input_queue.fetch();

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
        case GLFW_KEY_P :
#ifndef NDEBUG
            std::cerr << "Input: R (Reset)\n";
#endif
            paused = !paused;
            break;
    }


    if (view_component.should_window_close()) {
        close_game = true;
        return GLFW_KEY_ESCAPE; // Exit function, because quit command was invoked
    }

    sound_component.tick();

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


    // Row clearing
    // -----------
    // Clears rows and stores rows to move down in ranges

    struct RowMoveRanges {
        struct {
            int lowest_cleared_row;
            int highest_cleared_row;
        } range;

        int rows_cleared;
    };
    std::vector<RowMoveRanges> row_move_ranges;

    int lowest_cleared_row = -1;
    int highest_cleared_row = -1;
    bool create_range = false;
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

        // If row is full proceed to clear
        if (row_full) {

            // Store range of blocks to move down
            if (create_range) {
                row_move_ranges.push_back({{lowest_cleared_row, highest_cleared_row}, rows_cleared});

                // Reset counters
                lowest_cleared_row = -1;
                create_range = false;
            }

            ++rows_cleared;
            lowest_cleared_row = y - 1;
            for (int x = 0; x < GAME_WIDTH; ++x) {
                for (auto& l : landed) {
                    l.remove_block(glm::ivec2{x, y});
                }
            }

        } else {
            highest_cleared_row = y;
            if (lowest_cleared_row != -1) {
                create_range = true;
            }
        }
    }


    // Create final range
    // if necessary
    if (create_range) {
        row_move_ranges.push_back({{lowest_cleared_row, highest_cleared_row}, rows_cleared});
    }

    // Move down ranges
    // according to amount of rows cleared
    // when they were reached
    if (!row_move_ranges.empty()) {
        for (auto &r : row_move_ranges) {
            // Get all blocks above deleted row to move down
            std::vector<glm::ivec2> blocks_to_move_down;
            for (auto& l : landed) {
                auto blocks = l.get_blocks();
                for (auto& b : blocks) {
                    if (r.range.lowest_cleared_row >= b.y && r.range.highest_cleared_row <= b.y) {
                        blocks_to_move_down.push_back(b);
                    }
                }
                l.translate_blocks_down(blocks_to_move_down, r.rows_cleared);
                blocks_to_move_down.clear();
            }
        }
    }


    // Scoring
    // ------

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

    // Play sound according to how many rows were cleared
    if (rows_cleared >= 4) {
        sound_component.play_sfx(SoundUtil::SFXSound::TETRIS);
    } else if (rows_cleared >= 1 && rows_cleared < 4) {
        sound_component.play_sfx(SoundUtil::SFXSound::LINE_CLEAR);
    }

    // Freeing memory
    // --------------

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