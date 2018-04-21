// 
// Created by Balajanovski on 21/04/2018.
//

#ifndef INC_3D_TETRIS_TETROMINO_H
#define INC_3D_TETRIS_TETROMINO_H

#include "Util/EnumClassHash.h"

#include <unordered_map>
#include <array>
#include <glm/vec2.hpp>

namespace TetrominoUtil {
    static constexpr int BLOCKS_IN_TETROMINO = 4;

    enum class TetrominoType {
        LINE          = 0,
        L             = 1,
        REVERSE_L     = 2,
        STAIR         = 3,
        REVERSE_STAIR = 4,
        BLOCK         = 5,
    };
}

class Tetromino {
    Tetromino(TetrominoUtil::TetrominoType type);

    // Translation functions
    void translate_left();
    void translate_right();
    void translate_down();

    // Rotation functions
    void rotate_left();
    void rotate_right();
private:

    const static std::unordered_map<TetrominoUtil::TetrominoType,
            std::array< std::array<glm::ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO>, 4>, EnumClassHash> tetromino_rotations;

    glm::ivec2 top_left_point; // Point at the very top left of the
                               // imaginary 4x4 relative space tetrominos reside in
    std::array<glm::ivec2, TetrominoUtil::BLOCKS_IN_TETROMINO> blocks;

    TetrominoUtil::TetrominoType tetromino_type;
    int rotation_state;
};


#endif //INC_3D_TETRIS_TETROMINO_H
