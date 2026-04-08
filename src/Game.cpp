#include "Game.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Game: SDL_Init failed – %s", SDL_GetError());
        return false;
    }

    if (!renderer_.init()) return false;

    // Audio 
    audio_.init();

    loadHighScore();
    restart();
    lastTime_ = Clock::now();
    return true;
}

void Game::cleanup() {
    saveHighScore();
    audio_.cleanup();
    renderer_.cleanup();
    SDL_Quit();
}

void Game::run() {
    while (running_) {
        tick();

        // Coarse frame 
        SDL_Delay(1);
    }
}

void Game::tick() {
    if (!running_) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return;
    }

    // Delta time
    auto now = Clock::now();
    double dt = std::chrono::duration<double>(now - lastTime_).count();
    lastTime_ = now;
    if (dt > 0.25) dt = 0.25; 

    animTime_ += static_cast<float>(dt);

    processInput();

    // Fixed-timestep update
    if (state_ == GameState::Playing) {
        const double tickInterval = 1.0 / static_cast<double>(speed_);
        accumulator_ += dt;
        while (accumulator_ >= tickInterval) {
            update();
            accumulator_ -= tickInterval;
        }
    }

    render();
}


void Game::processInput() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {

        case SDL_EVENT_QUIT:
            running_ = false;
            return;

        case SDL_EVENT_WINDOW_RESIZED:
            renderer_.handleResize();
            break;

        case SDL_EVENT_KEY_DOWN: {
            if (ev.key.repeat) break; 

            const SDL_Scancode sc = ev.key.scancode;

            // Direction controls
            if (state_ == GameState::Playing) {
                if (sc == SDL_SCANCODE_UP    || sc == SDL_SCANCODE_W)
                    snake_.setDirection(Direction::Up);
                else if (sc == SDL_SCANCODE_DOWN  || sc == SDL_SCANCODE_S)
                    snake_.setDirection(Direction::Down);
                else if (sc == SDL_SCANCODE_LEFT  || sc == SDL_SCANCODE_A)
                    snake_.setDirection(Direction::Left);
                else if (sc == SDL_SCANCODE_RIGHT || sc == SDL_SCANCODE_D)
                    snake_.setDirection(Direction::Right);
            }

            // Pause toggle
            if (sc == SDL_SCANCODE_P || sc == SDL_SCANCODE_ESCAPE) {
                if (state_ == GameState::Playing)
                    state_ = GameState::Paused;
                else if (state_ == GameState::Paused)
                    state_ = GameState::Playing;
            }

            // Restart from game-over
            if (state_ == GameState::GameOver) {
                if (sc == SDL_SCANCODE_SPACE || sc == SDL_SCANCODE_RETURN)
                    restart();
            }

            // Theme 
            if (sc == SDL_SCANCODE_T)
                renderer_.toggleTheme();

            break;
        }

        
        case SDL_EVENT_FINGER_DOWN:
            touchStartX_ = ev.tfinger.x;
            touchStartY_ = ev.tfinger.y;
            touching_ = true;
            break;

        case SDL_EVENT_FINGER_UP: {
            if (!touching_) break;
            touching_ = false;

            float dx = ev.tfinger.x - touchStartX_;
            float dy = ev.tfinger.y - touchStartY_;
            constexpr float kSwipeThreshold = 0.03f; // normalized

            // Tap  restart on game over
            if (std::abs(dx) < kSwipeThreshold && std::abs(dy) < kSwipeThreshold) {
                if (state_ == GameState::GameOver) restart();
                else if (state_ == GameState::Paused) state_ = GameState::Playing;
                break;
            }

            if (state_ != GameState::Playing) break;

            if (std::abs(dx) > std::abs(dy)) {
                snake_.setDirection(dx > 0 ? Direction::Right : Direction::Left);
            } else {
                snake_.setDirection(dy > 0 ? Direction::Down : Direction::Up);
            }
            break;
        }

        default:
            break;
        }
    }
}


void Game::update() {
    snake_.update();

    // Wall / self collision → game over
    if (snake_.checkWallCollision() || snake_.checkSelfCollision()) {
        state_ = GameState::GameOver;
        audio_.playDie();
        if (score_ > highScore_) {
            highScore_ = score_;
            saveHighScore();
        }
        return;
    }

    // Food collision → grow + score + speed up
    if (snake_.getHead() == food_.getPosition()) {
        snake_.grow();
        food_.spawn(snake_.getBody());
        ++score_;
        speed_ = std::min(Config::MaxSpeed,
                          Config::InitialSpeed + Config::SpeedIncrement * score_);
        audio_.playEat();
    }
}

// ===========================================================================
// Render
// ===========================================================================

void Game::render() {
    const double tickInterval = 1.0 / static_cast<double>(speed_);
    float alpha = static_cast<float>(accumulator_ / tickInterval);
    alpha = std::clamp(alpha, 0.0f, 1.0f);

    renderer_.clear();
    renderer_.drawGrid();
    renderer_.drawFood(food_, animTime_);
    renderer_.drawSnake(snake_, (state_ == GameState::Playing) ? alpha : 1.0f);
    renderer_.drawScoreBar(score_, highScore_);

    if (state_ == GameState::Paused)
        renderer_.drawPauseOverlay();
    else if (state_ == GameState::GameOver)
        renderer_.drawGameOverOverlay(score_, highScore_);

    renderer_.present();
}


void Game::restart() {
    snake_.reset();
    food_.spawn(snake_.getBody());
    score_       = 0;
    speed_       = Config::InitialSpeed;
    accumulator_ = 0.0;
    animTime_    = 0.0f;
    state_       = GameState::Playing;
    lastTime_    = Clock::now();
}



std::string Game::highScorePath() const {
    char* pref = SDL_GetPrefPath("SnakeGame", "SnakeGame");
    if (pref) {
        std::string path = std::string(pref) + "highscore.txt";
        SDL_free(pref);
        return path;
    }
    return "highscore.txt"; // fallback to cwd
}

void Game::loadHighScore() {
    std::ifstream in(highScorePath());
    if (in.is_open()) in >> highScore_;
}

void Game::saveHighScore() {
    std::ofstream out(highScorePath());
    if (out.is_open()) out << highScore_;
}
