// 
// Created by Balajanovski on 2/06/2018.
//

#ifndef INC_3D_TETRIS_SOUNDCOMPONENT_H
#define INC_3D_TETRIS_SOUNDCOMPONENT_H

// Include OpenAL
#include "al.h"
#include "alc.h"

#include "Constants.h"

#include <string>
#include <vector>
#include <map>

namespace SoundUtil {
    enum SFXSound {
        TETRIS,
        LINE_CLEAR,
        MOVE,
    };
}

class Game;

class SoundComponent {
public:
    SoundComponent(const std::vector<std::string>& music_srcs,
                   const std::string& game_over_music_src,
                   const std::map<SoundUtil::SFXSound, std::string>& sfx_srcs,
                   Game* game);
    ~SoundComponent();

    void play_music();
    void play_game_over_music();
    void play_sfx(SoundUtil::SFXSound sfx);
    void tick();
private:
    Game* bound_game;

    int num_songs;
    int current_song;

    ALCdevice* device;
    ALCcontext* context;

    bool playing_game_over_mus = false;

    std::vector<ALuint> playing_music_sources;
    std::vector<ALuint> playing_sfx_sources;

    std::vector<ALuint> music_buffers;
    ALuint game_over_music_buffer;
    std::map<SoundUtil::SFXSound, ALuint> sfx_buffers;
};


#endif //INC_3D_TETRIS_SOUNDCOMPONENT_H
