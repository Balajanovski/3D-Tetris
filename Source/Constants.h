// 
// Created by Balajanovski on 21/04/2018.
//

#ifndef INC_3D_TETRIS_CONSTANTS_H
#define INC_3D_TETRIS_CONSTANTS_H

static constexpr unsigned int GAME_WIDTH  = 10;
static constexpr unsigned int GAME_HEIGHT = 18;

static constexpr unsigned int BLOCK_SIZE = 45;
static constexpr unsigned int SCREEN_WIDTH  = 454;
static constexpr unsigned int SCREEN_HEIGHT = 580;

static constexpr unsigned int FPS = 60;

static constexpr float ROTATION_INCREMENT = 0.007f;
static constexpr float DISTANCE_BETWEEN_CAMERA_AND_GAME = 2.6f;

static constexpr int MAX_NUM_SFX_SOURCES = 10;

// Seconds till the current tetromino moves down
static constexpr double TIME_BETWEEN_TETROMINO_MOVEMENTS = 0.5;

#endif //INC_3D_TETRIS_CONSTANTS_H
