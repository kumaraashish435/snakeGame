#include "Snake.h"

Snake::Snake() { reset(); }

void Snake::reset() {
    body_.clear();
    const int cx = Config::GridWidth  / 2;
    const int cy = Config::GridHeight / 2;
    // Head in the middle, two trailing segments to the left
    body_.push_back({cx,     cy});
    body_.push_back({cx - 1, cy});
    body_.push_back({cx - 2, cy});

    prevBody_       = body_;
    direction_      = Direction::Right;
    nextDirection_  = Direction::Right;
    pendingGrow_    = false;
}

void Snake::setDirection(Direction dir) {
    // Prevent 180-degree reversal
    if ((dir == Direction::Up    && direction_ == Direction::Down)  ||
        (dir == Direction::Down  && direction_ == Direction::Up)    ||
        (dir == Direction::Left  && direction_ == Direction::Right) ||
        (dir == Direction::Right && direction_ == Direction::Left))
        return;
    nextDirection_ = dir;
}

void Snake::update() {
    prevBody_  = body_;
    direction_ = nextDirection_;

    Vec2i head = body_.front();
    switch (direction_) {
        case Direction::Up:    --head.y; break;
        case Direction::Down:  ++head.y; break;
        case Direction::Left:  --head.x; break;
        case Direction::Right: ++head.x; break;
    }
    body_.insert(body_.begin(), head);

    if (pendingGrow_) {
        pendingGrow_ = false;
        // Keep sizes in sync for interpolation
        prevBody_.push_back(prevBody_.back());
    } else {
        body_.pop_back();
    }
}

void Snake::grow() { pendingGrow_ = true; }

bool Snake::checkWallCollision() const {
    const auto& h = body_.front();
    return h.x < 0 || h.x >= Config::GridWidth ||
           h.y < 0 || h.y >= Config::GridHeight;
}

bool Snake::checkSelfCollision() const {
    const auto& h = body_.front();
    for (std::size_t i = 1; i < body_.size(); ++i)
        if (body_[i] == h) return true;
    return false;
}
