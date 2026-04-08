#pragma once

#include "Types.h"
#include <vector>
#include <random>

class Food {
public:
    Food();

    // Place food on a random 
    void spawn(const std::vector<Vec2i>& snakeBody);

    Vec2i getPosition() const { return position_; }

private:
    Vec2i          position_{0, 0};
    std::mt19937   rng_;
};
