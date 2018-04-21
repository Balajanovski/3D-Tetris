// 
// Created by Balajanovski on 21/04/2018.
//

#include "Tetromino.h"

#include "Constants.h"

#include <array>

using namespace glm;

/*
 * Precomputed tables of tetromino rotations
 * Coordinates relative to tetromino itself
 */

const std::unordered_map<TetrominoUtil::TetrominoType,
                         std::array< std::array<ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO>, 4>,
                                 EnumClassHash> Tetromino::tetromino_rotations = {
        {TetrominoUtil::TetrominoType::LINE,
                std::array< std::array<ivec2, 4>, 4> {{
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}, ivec2{3, 1}}}, // First state
                        std::array<ivec2, 4>{{ivec2{2, 0}, ivec2{2, 1}, ivec2{2, 2}, ivec2{2, 3}}}, // Second state

                        // Repeated since rotation is the same
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}, ivec2{3, 1}}}, // First state
                        std::array<ivec2, 4>{{ivec2{2, 0}, ivec2{2, 1}, ivec2{2, 2}, ivec2{2, 3}}}, // Second state
                }}
        },
        {TetrominoUtil::TetrominoType::L,
                std::array< std::array<ivec2, 4>, 4>  {{
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}, ivec2{0, 2}}}, // First state
                        std::array<ivec2, 4>{{ivec2{0, 0}, ivec2{1, 0}, ivec2{1, 1}, ivec2{1, 2}}}, // Second state
                        std::array<ivec2, 4>{{ivec2{2, 0}, ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}}}, // Third state
                        std::array<ivec2, 4>{{ivec2{1, 0}, ivec2{1, 1}, ivec2{1, 2}, ivec2{2, 2}}}, // Fourth state
                }}
        },
        {TetrominoUtil::TetrominoType::REVERSE_L,
                std::array< std::array<ivec2, 4>, 4>  {{
                        std::array<ivec2, 4>{{ivec2{0, 0}, ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}}}, // First state
                        std::array<ivec2, 4>{{ivec2{1, 0}, ivec2{2, 0}, ivec2{1, 1}, ivec2{1, 2}}}, // Second state
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{2, 1}, ivec2{2, 2}}}, // Third state
                        std::array<ivec2, 4>{{ivec2{1, 0}, ivec2{1, 1}, ivec2{1, 2}, ivec2{0, 2}}}, // Fourth state
                }}
        },
        {TetrominoUtil::TetrominoType::STAIR,
                std::array< std::array<ivec2, 4>, 4>  {{
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{1, 2}, ivec2{2, 2}}}, // First state
                        std::array<ivec2, 4>{{ivec2{2, 0}, ivec2{2, 1}, ivec2{1, 1}, ivec2{1, 2}}}, // Second state

                        // Repeated since the rotation is the same
                        std::array<ivec2, 4>{{ivec2{0, 1}, ivec2{1, 1}, ivec2{1, 2}, ivec2{2, 2}}}, // First state
                        std::array<ivec2, 4>{{ivec2{2, 0}, ivec2{2, 1}, ivec2{1, 1}, ivec2{1, 2}}}, // Second state
                }}
        },
        {TetrominoUtil::TetrominoType::REVERSE_STAIR,
                std::array< std::array<ivec2, 4>, 4>  {{
                        std::array<ivec2, 4>{{ivec2{0, 2}, ivec2{1, 1}, ivec2{1, 2}, ivec2{2, 1}}}, // First state
                        std::array<ivec2, 4>{{ivec2{1, 0}, ivec2{1, 1}, ivec2{2, 1}, ivec2{2, 2}}}, // Second state

                        // Repeated since the rotation is the same
                        std::array<ivec2, 4>{{ivec2{0, 2}, ivec2{1, 1}, ivec2{1, 2}, ivec2{2, 1}}}, // First state
                        std::array<ivec2, 4>{{ivec2{1, 0}, ivec2{1, 1}, ivec2{2, 1}, ivec2{2, 2}}}, // Second state
                }}

        },
        {TetrominoUtil::TetrominoType::BLOCK,
                // All states are the same
                std::array< std::array<ivec2, 4>, 4>  {{
                        std::array<ivec2, 4>{{vec2{1, 1}, vec2{2, 1}, vec2{1, 2}, vec2{2, 2}}},
                        std::array<ivec2, 4>{{vec2{1, 1}, vec2{2, 1}, vec2{1, 2}, vec2{2, 2}}},
                        std::array<ivec2, 4>{{vec2{1, 1}, vec2{2, 1}, vec2{1, 2}, vec2{2, 2}}},
                        std::array<ivec2, 4>{{vec2{1, 1}, vec2{2, 1}, vec2{1, 2}, vec2{2, 2}}},
                }}
        },
};

Tetromino::Tetromino(TetrominoUtil::TetrominoType type) : tetromino_type(type), rotation_state(0)
{
    std::array<ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO> relative_coords =
            tetromino_rotations.find(type)->second[rotation_state];
    unsigned int half_game_width = GAME_WIDTH / 2;

    top_left_point = ivec2{half_game_width, 0};

    // Translate into game space (relative to game)
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_coords[i] += ivec2(half_game_width, 0);
    }

    blocks = relative_coords;
}

void Tetromino::translate_left() {
    // Check if block can be translated without hitting the wall
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        if (blocks[i].x < 1) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }

    // Translate block
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks[i].x -= 1;
    }
    top_left_point.x -= 1;
}

void Tetromino::translate_right() {
    // Check if block can be translated without hitting the wall
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        if (blocks[i].x > GAME_WIDTH - 2) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }

    // Translate block
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks[i].x += 1;
    }
    top_left_point.x += 1;
}

void Tetromino::translate_down() {
    // Check if block can be translated without hitting the wall
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        if (blocks[i].y > GAME_HEIGHT - 2) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }

    // Translate block
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks[i].y += 1;
    }
    top_left_point.y += 1;
}

void Tetromino::rotate_left() {
    // Update rotation_state
    rotation_state = (rotation_state > 0) ? rotation_state - 1 : 3;

    // Fetch relative positions of new rotation
    auto relative_position = tetromino_rotations.find(tetromino_type)->second[rotation_state];

    // Transform relative positions into game space
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_position[i] += top_left_point;
    }

    blocks = relative_position;

    // Wall kick
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        if (relative_position[i].x < 0) {
            translate_right();
        } else if (relative_position[i].x > GAME_WIDTH - 1) {
            translate_left();
        }
    }
}

void Tetromino::rotate_right() {
    // Update rotation_state
    rotation_state = (rotation_state < 3) ? rotation_state + 1 : 0;

    // Fetch relative positions of new rotation
    auto relative_position = tetromino_rotations.find(tetromino_type)->second[rotation_state];

    // Transform relative positions into game space
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_position[i] += top_left_point;
    }

    blocks = relative_position;

    // Wall kick
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        if (relative_position[i].x < 0) {
            translate_right();
        } else if (relative_position[i].x > GAME_WIDTH - 1) {
            translate_left();
        }
    }
}