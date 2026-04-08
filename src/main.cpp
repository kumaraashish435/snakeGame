#include "Game.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static std::unique_ptr<Game> g_game;

static void emscriptenTick() {
    g_game->tick();
}
#endif

// Entry point
int main(int /*argc*/, char* /*argv*/[]) {
#ifdef __EMSCRIPTEN__
    g_game = std::make_unique<Game>();
    if (!g_game->init()) {
        SDL_Log("Failed to initialise game");
        return 1;
    }
    // 0 = use requestAnimationFrame, 1 = simulate infinite loop
    emscripten_set_main_loop(emscriptenTick, 0, 1);
#else
    Game game;
    if (!game.init()) {
        SDL_Log("Failed to initialise game");
        return 1;
    }
    game.run();
    game.cleanup();
#endif
    return 0;
}
