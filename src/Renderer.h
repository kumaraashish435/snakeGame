#pragma once

#include "Types.h"
#include "Snake.h"
#include "Food.h"
#include <SDL3/SDL.h>

class Renderer {
public:
    Renderer()  = default;
    ~Renderer();

    bool init();
    void cleanup();

    // Frame lifecycle
    void clear();
    void present();

    // Drawing helpers
    void drawGrid();
    void drawSnake(const Snake& snake, float alpha);
    void drawFood(const Food& food, float animTime);
    void drawScoreBar(int score, int highScore);
    void drawPauseOverlay();
    void drawGameOverOverlay(int score, int highScore);

    // State
    void handleResize();
    void toggleTheme();

    SDL_Window* getWindow() const { return window_; }

private:
    // Primitive helpers
    void fillRect(float x, float y, float w, float h, Color c);
    void fillRectOutline(float x, float y, float w, float h, Color c, float t);

    // Pixel-art font (3×5 bitmap glyphs)
    void drawChar(char ch, float x, float y, float scale, Color c);
    void drawString(const char* str, float x, float y, float scale, Color c);
    float stringWidth(const char* str, float scale) const;

    // Grid → screen mapping
    float gx(float gridX) const;
    float gy(float gridY) const;

    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    int   winW_ = 0, winH_ = 0;
    float cellSz_  = 0.0f;
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;

    bool  dark_ = true;
    Theme theme_{};

    void applyTheme();

    static const Theme kDark;
    static const Theme kLight;
};
