#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>


class Audio {
public:
    Audio()  = default;
    ~Audio();

    bool init();        
    void playEat();
    void playDie();
    void cleanup();

private:
    void generateTone(std::vector<int16_t>& buf,
                      float startHz, float endHz,
                      float durationSec, float volume);
    void play(const std::vector<int16_t>& buf);

    SDL_AudioStream*      stream_ = nullptr;
    std::vector<int16_t>  eatBuf_;
    std::vector<int16_t>  dieBuf_;
    bool                  ready_ = false;

    static constexpr int kSampleRate = 44100;
};
