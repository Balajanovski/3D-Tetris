// 
// Created by Balajanovski on 21/04/2018.
//

#ifndef INC_3D_TETRIS_TETROMINO_H
#define INC_3D_TETRIS_TETROMINO_H

#include "Util/EnumClassHash.h"

#include <unordered_map>
#include <array>
#include <glm/vec2.hpp>
#include <set>
#include <vector>
#include <cstdint>

namespace TetrominoUtil {
    static constexpr int BLOCKS_IN_TETROMINO = 4;

    enum class TetrominoType {
        LINE = 0,
        L = 1,
        REVERSE_L = 2,
        STAIR = 3,
        REVERSE_STAIR = 4,
        BLOCK = 5,
    };

    enum class TetrominoState {
        MOVING = 0,
        LANDED = 1,
    };

    struct CompareIvec2 {
        const bool operator()(const glm::ivec2 &a, const glm::ivec2 &b) const;
    };
}

class Game;

class Tetromino {
public:
    Tetromino(TetrominoUtil::TetrominoType type, Game* game);
    Tetromino& operator=(const Tetromino& rhs);

    // Translation functions
    void translate_left();
    void translate_right();
    void translate_down();

    // Rotation functions
    void rotate_left();
    void rotate_right();

    void remove_blocks(const std::vector<glm::ivec2>& blocks_to_remove);

    bool is_block_part(const glm::ivec2 &block) const; // Check if block is a part of this tetromino
    bool tetromino_overlaps(const Tetromino& t) const; // Check if other tetromino overlaps this tetromino

    int highest_block() const; // Returns y coord of highest block in tetromino

    // Getters
    uint32_t get_color() const { return color; }
    const std::set<glm::ivec2, TetrominoUtil::CompareIvec2>& get_blocks() const { return blocks; }
private:
    int rotation_state;

    Game* bound_game; // The game which the Tetromino is a part of

    void land_tetromino();

    /*
     * Precomputed table of tetromino rotations
     * Coordinates relative to tetromino itself
     */
    const static std::unordered_map<TetrominoUtil::TetrominoType,
            std::array< std::array<glm::ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO>, 4>, EnumClassHash> tetromino_rotations;

    const static uint32_t possible_colors[5];

    glm::ivec2 top_left_point; // Point at the very top left of the
                               // imaginary 4x4 relative space tetrominos reside in
    std::set<glm::ivec2, TetrominoUtil::CompareIvec2> blocks;

    TetrominoUtil::TetrominoType tetromino_type;
    TetrominoUtil::TetrominoState tetromino_state;

    uint32_t color;
};

#endif //INC_3D_TETRIS_TETROMINO_H
