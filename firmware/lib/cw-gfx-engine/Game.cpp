#include "Game.h"
#include <Arduino.h>

void Game::addSprite(std::shared_ptr<Sprite> sprite) {
    _sprites.push_back(sprite);
}

void Game::removeSprite(size_t index) {
    if (index < _sprites.size()) {
        _sprites.erase(_sprites.begin() + index);
    }
}

void Game::setTickInterval(unsigned long interval_ms) {
    _tick_interval = interval_ms;
    // Update tick interval for all sprites
    for (auto& sprite : _sprites) {
        sprite->setTickInterval(interval_ms);
    }
}

void Game::update() {
    unsigned long current_time = millis();
    
    // Check if we should process a new tick
    if (current_time - _last_tick >= _tick_interval) {
        _last_tick = current_time;
        
        // Update all sprites
        for (auto& sprite : _sprites) {
            sprite->update();
        }
    }
}

const std::vector<std::shared_ptr<Sprite>>& Game::getSprites() const {
    return _sprites;
}

unsigned long Game::getTickInterval() const {
    return _tick_interval;
} 