#pragma once

#include <cstdint>



struct Vec2i {
    int x = 0;
    int y = 0;
    bool operator==(const Vec2i& o) const = default;
};


enum class Direction { Up, Down, Left, Right };



enum class GameState { Playing, Paused, GameOver };


struct Color {
    uint8_t r, g, b, a;
};


struct Theme {
    Color background;
    Color grid;
    Color snakeHead;
    Color snakeBody;
    Color snakeTail;   
    
    Color food;
    Color text;
    Color overlay;     
    
    Color accent;
};
