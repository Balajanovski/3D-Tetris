// 
// Created by Balajanovski on 2/06/2018.
//

#include "SoundComponent.h"
#include "Game.h"

#define DR_WAV_IMPLEMENTATION
#include <dr_wav/dr_wav.h>

#include <cstdlib>

#ifndef NDEBUG
#include <iostream>
#endif

// Util function to convert DR Wav format into OpenAL format
static inline ALenum to_al_format(short channels, short samples);

// Creates OpenAL buffer from source of WAV file
static void buffer_wav(const std::string& src, ALuint buffer);

// Util function to check for OpenAL errors
static void check_for_al_error();

SoundComponent::SoundComponent(const std::vector<std::string>& music_srcs,
                               const std::string& game_over_music_src,
                               const std::map<SoundUtil::SFXSound, std::string>& sfx_srcs,
                               Game* game)
        : num_songs(music_srcs.size()),
          music_buffers(num_songs),
          bound_game(game),
          current_song(bound_game->rng_component.rng(0, num_songs - 1))
{
    // Open Device
    // -----------
    const char* default_device_name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
#ifndef NDEBUG
    std::cerr << "SoundComponent: Chosen default device " << default_device_name << '\n';
#endif
    device = alcOpenDevice(default_device_name);
    if (!device) {
        throw std::runtime_error("OpenAL error: Device unable to be found");
    }
    check_for_al_error();

    // Create context
    // --------------
    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context)) {
        throw std::runtime_error("OpenAL error: failed to make default context");
    }
    check_for_al_error();

    // Set listener position
    // ---------------------
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    float listener_orientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    alListenerfv(AL_ORIENTATION, listener_orientation);
    check_for_al_error();

    // Generate sources
    // ---------------

    // Generate music source
    alGenSources(1, &music_source);
    check_for_al_error();
    alSourcei(music_source, AL_SOURCE_RELATIVE, AL_TRUE);
    check_for_al_error();
    alSource3f(music_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    alSourcef(music_source, AL_PITCH, 1);
    check_for_al_error();
    alSourcef(music_source, AL_GAIN, 0.5f);
    check_for_al_error();
    alSource3f(music_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    alSourcei(music_source, AL_LOOPING, AL_FALSE);
    check_for_al_error();



    // Create music buffers
    // -------------------
    alGenBuffers(num_songs, &music_buffers[0]);
    check_for_al_error();
    for (int i = 0; i < num_songs; ++i) {
        buffer_wav(music_srcs[i], music_buffers[i]);
    }
    // Create game over music buffer
    alGenBuffers(1, &game_over_music_buffer);
    buffer_wav(game_over_music_src, game_over_music_buffer);


    // Create SFX buffers
    // ------------------
    for (auto iter = sfx_srcs.begin(); iter != sfx_srcs.end(); ++iter) {
        GLuint new_buffer;
        alGenBuffers(1, &new_buffer);

        buffer_wav(iter->second, new_buffer);

        sfx_buffers.insert(std::make_pair(iter->first, new_buffer));
    }

}

SoundComponent::~SoundComponent() {
    alDeleteSources(1, &music_source);
    alDeleteBuffers(num_songs, &music_buffers[0]);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void SoundComponent::play_music() {
    if (playing_game_over_music) {
        alSourceStop(music_source);
        check_for_al_error();
        playing_game_over_music = false;
    } else {
        // Find out sound source state
        ALint source_state;
        alGetSourcei(music_source, AL_SOURCE_STATE, &source_state);
        check_for_al_error();

        if (source_state != AL_PLAYING) {
            // Bind music to sources
            alSourcei(music_source, AL_BUFFER, music_buffers[current_song]);
            check_for_al_error();

            alSourcePlay(music_source);
            check_for_al_error();

            ++current_song;
            if (current_song >= num_songs) {
                current_song = 0;
            }
        }
    }


}

void SoundComponent::play_game_over_music() {
    if (!playing_game_over_music) {
        alSourceStop(music_source);
        check_for_al_error();

        alSourcei(music_source, AL_BUFFER, game_over_music_buffer);
        check_for_al_error();

        alSourcePlay(music_source);
        check_for_al_error();

        playing_game_over_music = true;
    }
}

void SoundComponent::play_sfx(SoundUtil::SFXSound sfx) {


    // Generate sfx source
    ALuint new_source;
    alGenSources(1, &new_source);
    check_for_al_error();
    alSourcei(music_source, AL_SOURCE_RELATIVE, AL_TRUE);
    check_for_al_error();
    alSource3f(music_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    alSourcef(music_source, AL_PITCH, 1);
    check_for_al_error();
    alSourcef(music_source, AL_GAIN, 1.0f);
    check_for_al_error();
    alSource3f(music_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    check_for_al_error();
    alSourcei(music_source, AL_LOOPING, AL_FALSE);
    check_for_al_error();

    // Append sfx buffer to source
    alSourcei(new_source, AL_BUFFER, sfx_buffers.find(sfx)->second);
    check_for_al_error();

    // Play source
    alSourcePlay(new_source);
    check_for_al_error();

    // Add source to freeing queue
    if (sfx_sources_index < MAX_NUM_SFX_SOURCES) {
        sfx_sources[sfx_sources_index++] = new_source;
    }

}

void SoundComponent::tick() {
    for (int i = 0; i < MAX_NUM_SFX_SOURCES; ++i) {
        // Find out sound source state
        ALint source_state;
        alGetSourcei(sfx_sources[i], AL_SOURCE_STATE, &source_state);
        check_for_al_error();

        // Free source if not playing sound
        if (source_state == AL_STOPPED) {
            alDeleteSources(1, &sfx_sources[i]);
            check_for_al_error();
        }
    }
}

// Util function to convert dr_wav format into OpenAL format
static inline ALenum to_al_format(short channels, short samples) {
    bool stereo = (channels > 1);

    switch (samples) {
        case 16:
            if (stereo)
                return AL_FORMAT_STEREO16;
            else
                return AL_FORMAT_MONO16;
        case 8:
            if (stereo)
                return AL_FORMAT_STEREO8;
            else
                return AL_FORMAT_MONO8;
        default:
            return -1;
    }
}

// Creates OpenAL buffer from source of WAV file
static void buffer_wav(const std::string& src, ALuint buffer) {
    // Open WAV file
    drwav* wav = drwav_open_file(src.c_str());
    if (wav == nullptr) {
        throw std::runtime_error(std::string("dr_wav error: unable to open file - ") + src);
    }

    // Buffer WAV data
    auto sample_data = (int32_t*) std::malloc((size_t)wav->totalSampleCount * sizeof(int32_t));
    drwav_read_s32(wav, wav->totalSampleCount, sample_data);

    // Buffer the data in
    alBufferData(buffer, to_al_format(wav->channels, wav->bitsPerSample),
                 sample_data, (size_t)wav->totalSampleCount * sizeof(int32_t), wav->fmt.sampleRate * 2);
    check_for_al_error();

    // Cleanup
    free(sample_data);
    drwav_close(wav);
}

// Util function to check for OpenAL errors
static void check_for_al_error() {
    ALCenum error;
    error = alGetError();

    if (error != AL_NO_ERROR) {
        switch (error) {
            case AL_INVALID_NAME :
                throw std::runtime_error("OpenAL error with error code of: AL_INVALID_NAME");
                break;
            case AL_INVALID_ENUM :
                throw std::runtime_error("OpenAL error with error code of: AL_INVALID_ENUM");
                break;
            case AL_INVALID_VALUE :
                throw std::runtime_error("OpenAL error with error code of: AL_INVALID_VALUE");
                break;
            case AL_INVALID_OPERATION :
                throw std::runtime_error("OpenAL error with error code of: AL_INVALID_OPERATION");
                break;
            case AL_OUT_OF_MEMORY :
                throw std::runtime_error("OpenAL error with error code of: AL_OUT_OF_MEMORY");
                break;
        }
    }

}