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
    SoundComponent(const std::vector<std::string>& music_srcs, const std::string& game_over_music_src, Game* game);
    ~SoundComponent();

    void play_music();
    void play_game_over_music();
private:
    Game* bound_game;

    int num_songs;
    int current_song;
    bool playing_game_over_music = false;

    ALCdevice* device;
    ALCcontext* context;

    ALuint music_source;

    std::vector<ALuint> music_buffers;
    ALuint game_over_music_buffer;
};


#endif //INC_3D_TETRIS_SOUNDCOMPONENT_H
