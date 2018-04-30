// 
// Created by Balajanovski on 21/04/2018.
//

#include "Tetromino.h"

#include "Constants.h"
#include "Game.h"

#include <array>

using namespace glm;

/*
 * Precomputed table of tetromino rotations
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
                        std::array<ivec2, 4>{{ivec2{1, 1}, ivec2{2, 1}, ivec2{1, 2}, ivec2{2, 2}}},
                        std::array<ivec2, 4>{{ivec2{1, 1}, ivec2{2, 1}, ivec2{1, 2}, ivec2{2, 2}}},
                        std::array<ivec2, 4>{{ivec2{1, 1}, ivec2{2, 1}, ivec2{1, 2}, ivec2{2, 2}}},
                        std::array<ivec2, 4>{{ivec2{1, 1}, ivec2{2, 1}, ivec2{1, 2}, ivec2{2, 2}}},
                }}
        },
};

const uint32_t Tetromino::possible_colors[5] = {
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0x00FFFF, // Cyan
    0xFFDB58, // Mustard
};

Tetromino::Tetromino(TetrominoUtil::TetrominoType type, Game* game)
        : tetromino_type(type),
          tetromino_state(TetrominoUtil::TetrominoState::MOVING),
          rotation_state(0),
          bound_game(game),
          color(possible_colors[game->rng_component.rng(0, 4)])
{
    std::array<ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO> relative_coords =
            tetromino_rotations.find(type)->second[rotation_state];
    unsigned int half_game_width = GAME_WIDTH / 2;

    top_left_point = ivec2{half_game_width, 0};

    // Translate into game space (relative to game)
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_coords[i] += ivec2(half_game_width, 0);
    }

    // Copy relative coords into blocks vector
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks.insert(relative_coords[i]);
    }
}

Tetromino& Tetromino::operator=(const Tetromino &rhs) {
    bound_game      = rhs.bound_game;
    rotation_state  = rhs.rotation_state;
    top_left_point  = rhs.top_left_point;
    blocks          = rhs.blocks;
    tetromino_type  = rhs.tetromino_type;
    tetromino_state = rhs.tetromino_state;

    return *this;
}

void Tetromino::translate_left() {
    if (tetromino_state == TetrominoUtil::TetrominoState::LANDED) {
        return; // End function if the tetromino has landed
    }

    // Check if block can be translated without hitting the wall
    for (auto iter = blocks.cbegin(); iter != blocks.cend(); ++iter) {
        if (iter->x < 1) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }


    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> old_blocks = blocks; // Store blocks before translation

    // Translate block
    for (auto b: blocks) {
        b.x -= 1;
    }

    if (!bound_game->check_collision(*this)) {
        top_left_point.x -= 1;      // Translate the top left point
                                    // because it is ok to translate
    } else {
        blocks = old_blocks;        // Revert the translation
    }
}

void Tetromino::translate_right() {
    if (tetromino_state == TetrominoUtil::TetrominoState::LANDED) {
        return; // End function if the tetromino has landed
    }

    // Check if block can be translated without hitting the wall
    for (auto iter = blocks.cbegin(); iter != blocks.cend(); ++iter) {
        if (iter->x > GAME_WIDTH - 2) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }

    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> old_blocks = blocks; // Store blocks before translation

    // Translate block
    for (auto b: blocks) {
        b.x += 1;
    }
    if (!bound_game->check_collision(*this)) {
        top_left_point.x += 1;      // Translate the top left point
                                    // because it is ok to translate
    } else {
        blocks = old_blocks;        // Revert the translation
    }
}

void Tetromino::translate_down() {
    // Check if block can be translated without hitting the wall
    for (auto iter = blocks.cbegin(); iter != blocks.cend(); ++iter) {
        if (iter->y > GAME_HEIGHT - 2) {
            return; // Exit the function because translation is impossible
                    // without hitting the wall
        }
    }

    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> old_blocks = blocks; // Store blocks before translation

    // Translate block
    for (auto b: blocks) {
        b.y += 1;
    }

    if (!bound_game->check_collision(*this)) {
        top_left_point.y += 1;      // Translate the top left point
                                    // because it is ok to translate
    } else {
        blocks = old_blocks;        // Revert the translation
        land_tetromino();           // Set the block as landed
    }
}

void Tetromino::rotate_left() {
    if (tetromino_state == TetrominoUtil::TetrominoState::LANDED) {
        return; // End function if the tetromino has landed for landed tetromino cannot be rotated
    }

    // Update rotation_state
    rotation_state = (rotation_state > 0) ? rotation_state - 1 : 3;

    // Fetch relative positions of new rotation
    auto relative_coords = tetromino_rotations.find(tetromino_type)->second[rotation_state];

    // Transform relative positions into game space
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_coords[i] += top_left_point;
    }

    // Store old block positions
    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> old_blocks = blocks;

    // Copy relative coords into blocks vector
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks.insert(relative_coords[i]);
    }

    if (!bound_game->check_collision(*this)) {
        // Wall kick
        for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
            if (relative_coords[i].x < 0) {
                translate_right();
            } else if (relative_coords[i].x > GAME_WIDTH - 1) {
                translate_left();
            }
        }
    } else {
        blocks = old_blocks;
    }
}

void Tetromino::rotate_right() {
    if (tetromino_state == TetrominoUtil::TetrominoState::LANDED) {
        return; // End function if the tetromino has landed for landed tetromino cannot be rotated
    }

    // Update rotation_state
    rotation_state = (rotation_state < 3) ? rotation_state + 1 : 0;

    // Fetch relative positions of new rotation
    auto relative_coords = tetromino_rotations.find(tetromino_type)->second[rotation_state];

    // Transform relative positions into game space
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        relative_coords[i] += top_left_point;
    }

    // Store old block positions
    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> old_blocks = blocks;

    // Copy relative coords into blocks vector
    for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
        blocks.insert(relative_coords[i]);
    }

    if (!bound_game->check_collision(*this)) {
        // Wall kick
        for (int i = 0; i < TetrominoUtil::BLOCKS_IN_TETROMINO; ++i) {
            if (relative_coords[i].x < 0) {
                translate_right();
            } else if (relative_coords[i].x > GAME_WIDTH - 1) {
                translate_left();
            }
        }
    } else {
        blocks = old_blocks;
    }
}

void Tetromino::remove_blocks(const std::vector<glm::ivec2>& blocks_to_remove) {
    if (tetromino_state != TetrominoUtil::TetrominoState::LANDED) {
        return; // End function if the tetromino has not landed yet
    }

    for (auto block_to_remove : blocks_to_remove) {
        blocks.erase(block_to_remove);
    }

    translate_down();
}

void Tetromino::land_tetromino() {
    tetromino_state = TetrominoUtil::TetrominoState::LANDED;

    bound_game->add_landed(*this);
}

bool Tetromino::is_block_part(const glm::ivec2 &block) const {
    return blocks.find(block) != blocks.end();
}

bool Tetromino::tetromino_overlaps(const Tetromino &t) const {
    for (auto iter = t.blocks.cbegin(); iter != t.blocks.cend(); ++iter) {
        if (is_block_part(*iter)) {
            return true;
        }
    }
    return false;
}

int Tetromino::highest_block() const {
    if (blocks.empty()) {
        throw std::runtime_error("error: highest_block() called on tetromino with no blocks");
    }

    int highest_block = blocks.cbegin()->y;
    for (auto iter = ++blocks.cbegin(); iter != blocks.cend(); ++iter) {
        if (iter->y < highest_block) {
            highest_block = iter->y;
        }
    }

    return highest_block;
}

const bool TetrominoUtil::CompareIvec2::operator()(const glm::ivec2 &a, const glm::ivec2 &b) const {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}