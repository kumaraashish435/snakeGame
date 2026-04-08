#include "Audio.h"
#include <cmath>
#include <numbers>

Audio::~Audio() { cleanup(); }

bool Audio::init() {
    if (ready_) return true;

    SDL_AudioSpec spec{};
    spec.format   = SDL_AUDIO_S16;
    spec.channels = 1;
    spec.freq     = kSampleRate;

    stream_ = SDL_OpenAudioDeviceStream(
                  SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!stream_) {
        SDL_Log("Audio: could not open device – %s", SDL_GetError());
        return false;
    }
    SDL_ResumeAudioStreamDevice(stream_);

    // Pre-generate sound buffers
    generateTone(eatBuf_, 320.0f, 640.0f, 0.10f, 0.30f);   // rising chirp
    generateTone(dieBuf_, 440.0f, 110.0f, 0.30f, 0.30f);   // descending buzz

    ready_ = true;
    return true;
}

void Audio::generateTone(std::vector<int16_t>& buf,
                          float startHz, float endHz,
                          float durationSec, float volume) {
    const int samples = static_cast<int>(kSampleRate * durationSec);
    buf.resize(samples);

    constexpr float kTwoPi = 2.0f * std::numbers::pi_v<float>;
    float phase = 0.0f;

    for (int i = 0; i < samples; ++i) {
        const float t    = static_cast<float>(i) / static_cast<float>(samples);
        const float freq = startHz + (endHz - startHz) * t;
        const float env  = 1.0f - t;  // linear fade-out

        phase += kTwoPi * freq / static_cast<float>(kSampleRate);
        if (phase > kTwoPi) phase -= kTwoPi;

        const float sample = std::sin(phase) * env * volume;
        buf[i] = static_cast<int16_t>(sample * 32767.0f);
    }
}

void Audio::play(const std::vector<int16_t>& buf) {
    if (!stream_ || buf.empty()) return;
    SDL_PutAudioStreamData(stream_, buf.data(),
                           static_cast<int>(buf.size() * sizeof(int16_t)));
}

void Audio::playEat() { play(eatBuf_); }
void Audio::playDie() { play(dieBuf_); }

void Audio::cleanup() {
    if (stream_) {
        SDL_DestroyAudioStream(stream_);
        stream_ = nullptr;
    }
    ready_ = false;
}
