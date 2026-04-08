#pragma once

#include "Types.h"
#include "Config.h"
#include <vector>

class Snake {
public:
    Snake();

    void reset();
    void setDirection(Direction dir);
    void update();                       
    
    void grow();                         
    

    bool checkWallCollision() const;
    bool checkSelfCollision() const;

    // Accessors
    const std::vector<Vec2i>& getBody()         const { return body_; }
    const std::vector<Vec2i>& getPreviousBody() const { return prevBody_; }
    Vec2i                     getHead()          const { return body_.front(); }
    Direction                 getDirection()      const { return direction_; }

private:
    std::vector<Vec2i> body_;
    std::vector<Vec2i> prevBody_;
    Direction direction_     = Direction::Right;
    Direction nextDirection_  = Direction::Right;
    bool      pendingGrow_   = false;
};
