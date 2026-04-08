#pragma once

#include "Snake.h"
#include "Food.h"
#include "Renderer.h"
#include "Audio.h"
#include "Config.h"
#include "Types.h"

#include <chrono>
#include <string>

class Game {
public:
    Game()  = default;
    ~Game() = default;

    bool init();
    void run();       
    void tick();      // Single iteration 
    void cleanup();

private:
    void processInput();
    void update();
    void render();
    void restart();

    void loadHighScore();
    void saveHighScore();
    std::string highScorePath() const;

    // Subsystems
    Snake    snake_;
    Food     food_;
    Renderer renderer_;
    Audio    audio_;

    // State
    GameState state_     = GameState::Playing;
    int       score_     = 0;
    int       highScore_ = 0;
    float     speed_     = Config::InitialSpeed;
    bool      running_   = true;

    // Timing 
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point lastTime_{};
    double accumulator_ = 0.0;
    float  animTime_    = 0.0f;

    // Touch input 
    float touchStartX_ = 0.0f;
    float touchStartY_ = 0.0f;
    bool  touching_     = false;
};
