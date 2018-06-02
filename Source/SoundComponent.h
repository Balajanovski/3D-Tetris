// 
// Created by Balajanovski on 2/06/2018.
//

#ifndef INC_3D_TETRIS_SOUNDCOMPONENT_H
#define INC_3D_TETRIS_SOUNDCOMPONENT_H

// Include OpenAL
#include "al.h"
#include "alc.h"

#include <string>
#include <vector>

class Game;

class SoundComponent {
public:
    SoundComponent(const std::vector<std::string>& music_srcs, Game* game);
    ~SoundComponent();

    void play_music();
private:
    Game* bound_game;

    int num_songs;
    int current_song;

    ALCdevice* device;
    ALCcontext* context;

    // Sound source
    ALuint source;

    std::vector<ALuint> music_buffers;
};


#endif //INC_3D_TETRIS_SOUNDCOMPONENT_H
