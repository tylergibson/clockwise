#pragma once

#include <vector>
#include <memory>
#include "Sprite.h"

const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 64;

enum Direction {
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Game {
  private:
    // Sprite management
    std::vector<std::shared_ptr<Sprite>> _sprites;
    unsigned long _last_tick = 0;
    unsigned long _tick_interval = 16; // ~60fps default
    
  public:
    // Sprite management
    void addSprite(std::shared_ptr<Sprite> sprite);
    void removeSprite(size_t index);
    void setTickInterval(unsigned long interval_ms);
    void update();
    
    // Getters
    const std::vector<std::shared_ptr<Sprite>>& getSprites() const;
    unsigned long getTickInterval() const;
};



