#pragma once

#include "Locator.h"
#include "Sprite.h"
#include <memory>

enum class TileDirection {
    RIGHT,
    LEFT,
    UP,
    DOWN
};

struct Tile {
    std::shared_ptr<Sprite> _sprite;
    int _spacing;
    TileDirection _direction;

    Tile(int spacing = 0, TileDirection direction = TileDirection::RIGHT) 
        : _sprite(nullptr), _spacing(spacing), _direction(direction) {}

    void setSprite(std::shared_ptr<Sprite> sprite) {
        _sprite = sprite;
    }

    void draw(int x, int y, int count) {
        if (!_sprite) return;

        int spriteWidth = _sprite->getWidth();
        int spriteHeight = _sprite->getHeight();

        for (int i = 0; i < count; i++) {  // Start from 1 to skip first instance
            int drawX = x;
            int drawY = y;

            switch (_direction) {
                case TileDirection::RIGHT:
                    drawX = x + i * (spriteWidth + _spacing);
                    break;
                case TileDirection::LEFT:
                    drawX = x - i * (spriteWidth + _spacing);
                    break;
                case TileDirection::DOWN:
                    drawY = y + i * (spriteHeight + _spacing);
                    break;
                case TileDirection::UP:
                    drawY = y - i * (spriteHeight + _spacing);
                    break;
            }

            _sprite->setPosition(drawX, drawY);
            Locator::getCanvas()->drawRGBBitmap(drawX, drawY, _sprite->getCurrentSprite(), _sprite->getCurrentMask(), spriteWidth, spriteHeight);
        }
    }
};
