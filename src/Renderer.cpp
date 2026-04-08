#include "Renderer.h"
#include "Config.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numbers>

// ===== Themes =============================================================

const Theme Renderer::kDark {
    /* background */ {0x1a, 0x1a, 0x2e, 0xff},
    /* grid       */ {0x16, 0x21, 0x3e, 0xff},
    /* snakeHead  */ {0x00, 0xff, 0x41, 0xff},
    /* snakeBody  */ {0x00, 0xcc, 0x33, 0xff},
    /* snakeTail  */ {0x00, 0x66, 0x22, 0xff},
    /* food       */ {0xff, 0x00, 0x40, 0xff},
    /* text       */ {0xff, 0xff, 0xff, 0xff},
    /* overlay    */ {0x00, 0x00, 0x00, 0xb0},
    /* accent     */ {0xe9, 0x45, 0x60, 0xff},
};

const Theme Renderer::kLight {
    /* background */ {0xf0, 0xf0, 0xf0, 0xff},
    /* grid       */ {0xdd, 0xdd, 0xdd, 0xff},
    /* snakeHead  */ {0x2d, 0x6a, 0x4f, 0xff},
    /* snakeBody  */ {0x40, 0x91, 0x6c, 0xff},
    /* snakeTail  */ {0x95, 0xd5, 0xb2, 0xff},
    /* food       */ {0xe6, 0x39, 0x46, 0xff},
    /* text       */ {0x1d, 0x35, 0x57, 0xff},
    /* overlay    */ {0xff, 0xff, 0xff, 0xb0},
    /* accent     */ {0x45, 0x7b, 0x9d, 0xff},
};


static constexpr uint8_t kDigits[10][5] = {
    {7,5,5,5,7}, // 0
    {2,6,2,2,7}, // 1
    {7,1,7,4,7}, // 2
    {7,1,7,1,7}, // 3
    {5,5,7,1,1}, // 4
    {7,4,7,1,7}, // 5
    {7,4,7,5,7}, // 6
    {7,1,1,1,1}, // 7
    {7,5,7,5,7}, // 8
    {7,5,7,1,7}, // 9
};

static constexpr uint8_t kAlpha[26][5] = {
    {7,5,7,5,5}, // A
    {6,5,6,5,6}, // B
    {7,4,4,4,7}, // C
    {6,5,5,5,6}, // D
    {7,4,7,4,7}, // E
    {7,4,7,4,4}, // F
    {7,4,5,5,7}, // G
    {5,5,7,5,5}, // H
    {7,2,2,2,7}, // I
    {1,1,1,5,7}, // J
    {5,6,4,6,5}, // K
    {4,4,4,4,7}, // L
    {5,7,7,5,5}, // M
    {5,7,5,5,5}, // N
    {7,5,5,5,7}, // O
    {7,5,7,4,4}, // P
    {7,5,5,7,1}, // Q
    {7,5,7,6,5}, // R
    {7,4,7,1,7}, // S
    {7,2,2,2,2}, // T
    {5,5,5,5,7}, // U
    {5,5,5,5,2}, // V
    {5,5,7,7,5}, // W
    {5,5,2,5,5}, // X
    {5,5,2,2,2}, // Y
    {7,1,2,4,7}, // Z
};

static constexpr int kGlyphW = 3;
static constexpr int kGlyphH = 5;

// ===== Init / Cleanup =====================================================

Renderer::~Renderer() { cleanup(); }

bool Renderer::init() {
    window_ = SDL_CreateWindow("Snake Game",
                               Config::InitialWindowWidth,
                               Config::InitialWindowHeight,
                               SDL_WINDOW_RESIZABLE);
    if (!window_) {
        SDL_Log("Renderer: SDL_CreateWindow failed – %s", SDL_GetError());
        return false;
    }

    SDL_SetWindowMinimumSize(window_, 320, 340);

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        SDL_Log("Renderer: SDL_CreateRenderer failed – %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    applyTheme();
    handleResize();
    return true;
}

void Renderer::cleanup() {
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_)   { SDL_DestroyWindow(window_);     window_   = nullptr; }
}

// ===== Theme ==============================================================

void Renderer::applyTheme() {
    theme_ = dark_ ? kDark : kLight;
}

void Renderer::toggleTheme() {
    dark_ = !dark_;
    applyTheme();
}




void Renderer::handleResize() {
    SDL_GetWindowSize(window_, &winW_, &winH_);

    const float margin = Config::ScoreBarHeight;
    const float availH = static_cast<float>(winH_) - margin;
    const float cellW  = static_cast<float>(winW_)  / Config::GridWidth;
    const float cellH  = availH / Config::GridHeight;
    cellSz_ = std::min(cellW, cellH);

    const float gridW = cellSz_ * Config::GridWidth;
    const float gridH = cellSz_ * Config::GridHeight;
    offsetX_ = (static_cast<float>(winW_) - gridW) * 0.5f;
    offsetY_ = margin + (availH - gridH) * 0.5f;
}

float Renderer::gx(float gridX) const { return offsetX_ + gridX * cellSz_; }
float Renderer::gy(float gridY) const { return offsetY_ + gridY * cellSz_; }


void Renderer::fillRect(float x, float y, float w, float h, Color c) {
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    SDL_FRect r{x, y, w, h};
    SDL_RenderFillRect(renderer_, &r);
}

void Renderer::fillRectOutline(float x, float y, float w, float h, Color c, float t) {
    fillRect(x,         y,         w, t, c);     // top
    fillRect(x,         y + h - t, w, t, c);     // bottom
    fillRect(x,         y + t,     t, h - 2*t, c); // left
    fillRect(x + w - t, y + t,     t, h - 2*t, c); // right
}

// ===== Pixel-art font =====================================================

void Renderer::drawChar(char ch, float x, float y, float scale, Color c) {
    const uint8_t* glyph = nullptr;

    if (ch >= '0' && ch <= '9')      glyph = kDigits[ch - '0'];
    else if (ch >= 'A' && ch <= 'Z') glyph = kAlpha[ch - 'A'];
    else if (ch >= 'a' && ch <= 'z') glyph = kAlpha[ch - 'a'];
    else return; // unsupported character – skip

    for (int row = 0; row < kGlyphH; ++row) {
        for (int col = 0; col < kGlyphW; ++col) {
            if (glyph[row] & (1 << (kGlyphW - 1 - col))) {
                fillRect(x + col * scale, y + row * scale, scale, scale, c);
            }
        }
    }
}

void Renderer::drawString(const char* str, float x, float y, float scale, Color c) {
    const float advance = (kGlyphW + 1) * scale; // glyph width + 1px gap
    float cx = x;
    for (const char* p = str; *p; ++p) {
        if (*p == ' ') { cx += advance; continue; }
        if (*p == ':') {
            // Draw colon – two dots
            fillRect(cx + scale, y + scale,         scale, scale, c);
            fillRect(cx + scale, y + 3.0f * scale,  scale, scale, c);
            cx += advance;
            continue;
        }
        drawChar(*p, cx, y, scale, c);
        cx += advance;
    }
}

float Renderer::stringWidth(const char* str, float scale) const {
    const float advance = (kGlyphW + 1) * scale;
    return static_cast<float>(std::strlen(str)) * advance - scale; // minus trailing gap
}


void Renderer::clear() {
    const auto& bg = theme_.background;
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}


void Renderer::drawGrid() {
    const Color& gc = theme_.grid;
    const float lineW = 1.0f;

    // Vertical lines
    for (int x = 0; x <= Config::GridWidth; ++x)
        fillRect(gx(static_cast<float>(x)) - lineW * 0.5f, gy(0),
                 lineW, cellSz_ * Config::GridHeight, gc);
    // Horizontal lines
    for (int y = 0; y <= Config::GridHeight; ++y)
        fillRect(gx(0), gy(static_cast<float>(y)) - lineW * 0.5f,
                 cellSz_ * Config::GridWidth, lineW, gc);
}


void Renderer::drawSnake(const Snake& snake, float alpha) {
    const auto& body = snake.getBody();
    const auto& prev = snake.getPreviousBody();
    const auto  dir  = snake.getDirection();
    const float pad  = cellSz_ * Config::CellPadding;
    const int   n    = static_cast<int>(body.size());

    for (int i = 0; i < n; ++i) {
        // Interpolate between previous and current position
        float fx, fy;
        if (i < static_cast<int>(prev.size())) {
            fx = prev[i].x + (body[i].x - prev[i].x) * alpha;
            fy = prev[i].y + (body[i].y - prev[i].y) * alpha;
        } else {
            fx = static_cast<float>(body[i].x);
            fy = static_cast<float>(body[i].y);
        }

        // Gradient colour: head → body → tail
        float t = (n > 1) ? static_cast<float>(i) / (n - 1) : 0.0f;
        Color col;
        if (t < 0.5f) {
            float u = t * 2.0f;
            col.r = static_cast<uint8_t>(theme_.snakeHead.r + (theme_.snakeBody.r - theme_.snakeHead.r) * u);
            col.g = static_cast<uint8_t>(theme_.snakeHead.g + (theme_.snakeBody.g - theme_.snakeHead.g) * u);
            col.b = static_cast<uint8_t>(theme_.snakeHead.b + (theme_.snakeBody.b - theme_.snakeHead.b) * u);
            col.a = 255;
        } else {
            float u = (t - 0.5f) * 2.0f;
            col.r = static_cast<uint8_t>(theme_.snakeBody.r + (theme_.snakeTail.r - theme_.snakeBody.r) * u);
            col.g = static_cast<uint8_t>(theme_.snakeBody.g + (theme_.snakeTail.g - theme_.snakeBody.g) * u);
            col.b = static_cast<uint8_t>(theme_.snakeBody.b + (theme_.snakeTail.b - theme_.snakeBody.b) * u);
            col.a = 255;
        }

        float p = (i == 0) ? pad * 0.3f : pad; // head is slightly larger
        fillRect(gx(fx) + p, gy(fy) + p, cellSz_ - 2 * p, cellSz_ - 2 * p, col);
    }

    // Draw eyes on the head
    if (body.empty()) return;
    float hx, hy;
    if (!prev.empty()) {
        hx = prev[0].x + (body[0].x - prev[0].x) * alpha;
        hy = prev[0].y + (body[0].y - prev[0].y) * alpha;
    } else {
        hx = static_cast<float>(body[0].x);
        hy = static_cast<float>(body[0].y);
    }

    const float esz = cellSz_ * 0.14f;    // eye size
    const Color eyeCol = theme_.background; // eyes match background
    float ex1, ey1, ex2, ey2;

    switch (dir) {
        case Direction::Right:
            ex1 = gx(hx) + cellSz_ * 0.65f; ey1 = gy(hy) + cellSz_ * 0.20f;
            ex2 = gx(hx) + cellSz_ * 0.65f; ey2 = gy(hy) + cellSz_ * 0.60f;
            break;
        case Direction::Left:
            ex1 = gx(hx) + cellSz_ * 0.20f; ey1 = gy(hy) + cellSz_ * 0.20f;
            ex2 = gx(hx) + cellSz_ * 0.20f; ey2 = gy(hy) + cellSz_ * 0.60f;
            break;
        case Direction::Up:
            ex1 = gx(hx) + cellSz_ * 0.20f; ey1 = gy(hy) + cellSz_ * 0.20f;
            ex2 = gx(hx) + cellSz_ * 0.60f; ey2 = gy(hy) + cellSz_ * 0.20f;
            break;
        case Direction::Down:
            ex1 = gx(hx) + cellSz_ * 0.20f; ey1 = gy(hy) + cellSz_ * 0.65f;
            ex2 = gx(hx) + cellSz_ * 0.60f; ey2 = gy(hy) + cellSz_ * 0.65f;
            break;
    }
    fillRect(ex1, ey1, esz, esz, eyeCol);
    fillRect(ex2, ey2, esz, esz, eyeCol);
}


void Renderer::drawFood(const Food& food, float animTime) {
    const Vec2i pos = food.getPosition();
    // Gentle pulsing (80 – 100 % of cell)
    const float pulse = 0.80f + 0.20f * std::sin(animTime * 5.0f);
    const float sz    = cellSz_ * pulse;
    const float off   = (cellSz_ - sz) * 0.5f;

    fillRect(gx(static_cast<float>(pos.x)) + off,
             gy(static_cast<float>(pos.y)) + off,
             sz, sz, theme_.food);

    // Small inner highlight for a "3-D" feel
    const float hi = sz * 0.3f;
    Color highlight = theme_.food;
    highlight.r = static_cast<uint8_t>(std::min(255, highlight.r + 60));
    highlight.g = static_cast<uint8_t>(std::min(255, highlight.g + 60));
    highlight.b = static_cast<uint8_t>(std::min(255, highlight.b + 60));
    fillRect(gx(static_cast<float>(pos.x)) + off + sz * 0.15f,
             gy(static_cast<float>(pos.y)) + off + sz * 0.15f,
             hi, hi, highlight);
}

// ===== HUD (score bar) ====================================================

void Renderer::drawScoreBar(int score, int highScore) {
    const float s = std::max(3.0f, cellSz_ * 0.18f); // pixel scale for font

    char buf[64];

    // Score – left-aligned
    std::snprintf(buf, sizeof(buf), "SCORE:%d", score);
    drawString(buf, offsetX_, (Config::ScoreBarHeight - kGlyphH * s) * 0.5f, s, theme_.text);

    // High score – right-aligned
    std::snprintf(buf, sizeof(buf), "HI:%d", highScore);
    float hw = stringWidth(buf, s);
    drawString(buf, offsetX_ + cellSz_ * Config::GridWidth - hw,
               (Config::ScoreBarHeight - kGlyphH * s) * 0.5f, s, theme_.accent);
}


void Renderer::drawPauseOverlay() {
    fillRect(0, 0, static_cast<float>(winW_), static_cast<float>(winH_), theme_.overlay);

    const float s = std::max(5.0f, cellSz_ * 0.4f);
    const char* msg = "PAUSED";
    float w = stringWidth(msg, s);
    drawString(msg,
               (winW_ - w) * 0.5f,
               (winH_ - kGlyphH * s) * 0.5f,
               s, theme_.text);
}

void Renderer::drawGameOverOverlay(int score, int highScore) {
    fillRect(0, 0, static_cast<float>(winW_), static_cast<float>(winH_), theme_.overlay);

    const float bigS   = std::max(5.0f, cellSz_ * 0.45f);
    const float smallS = std::max(3.0f, cellSz_ * 0.22f);
    const float cy     = static_cast<float>(winH_) * 0.5f;

    // "GAME OVER"
    {
        const char* msg = "GAME OVER";
        float w = stringWidth(msg, bigS);
        drawString(msg, (winW_ - w) * 0.5f, cy - bigS * kGlyphH * 1.5f, bigS, theme_.accent);
    }

    // Score
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "SCORE:%d", score);
        float w = stringWidth(buf, smallS);
        drawString(buf, (winW_ - w) * 0.5f, cy, smallS, theme_.text);
    }

    // High score
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "HI:%d", highScore);
        float w = stringWidth(buf, smallS);
        drawString(buf, (winW_ - w) * 0.5f, cy + smallS * (kGlyphH + 2), smallS, theme_.text);
    }

    // Restart hint
    {
        const char* hint = "PRESS SPACE";
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
        hint = "TAP TO RESTART";
#endif
        float w = stringWidth(hint, smallS);
        drawString(hint, (winW_ - w) * 0.5f, cy + smallS * (kGlyphH + 2) * 2.5f, smallS, theme_.text);
    }
}
