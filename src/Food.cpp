#include "Food.h"
#include "Config.h"
#include <algorithm>

Food::Food() : rng_(std::random_device{}()) {}

void Food::spawn(const std::vector<Vec2i>& snakeBody) {
    //  pick one at random
    std::vector<Vec2i> free;
    free.reserve(Config::GridWidth * Config::GridHeight);

    for (int y = 0; y < Config::GridHeight; ++y) {
        for (int x = 0; x < Config::GridWidth; ++x) {
            Vec2i cell{x, y};
            if (std::find(snakeBody.begin(), snakeBody.end(), cell) == snakeBody.end())
                free.push_back(cell);
        }
    }

    if (!free.empty()) {
        std::uniform_int_distribution<std::size_t> dist(0, free.size() - 1);
        position_ = free[dist(rng_)];
    }
}
