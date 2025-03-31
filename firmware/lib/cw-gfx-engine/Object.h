#pragma once
#include <vector>
#include <cstdint>
#include "Locator.h"

struct Object {
  const unsigned short *_image;
  size_t _image_size;
  std::vector<uint8_t> _mask;
  size_t _width;
  size_t _height;

  Object(const unsigned short *image, short *config) {
    this->_image = image;
    createBitMask(config[3]);
    this->_image_size = config[2];
    this->_width = config[0];
    this->_height = config[1];
  }

  void createBitMask(unsigned short maskValue) {
    // Resize and clear the mask vector
    _mask.resize((_image_size + 7) / 8, 0);
    std::fill(_mask.begin(), _mask.end(), 0);
    
    for (size_t i = 0; i < _image_size; ++i) {
        // Calculate which byte and bit position we're working with
        size_t byteIndex = i / 8;
        size_t bitPosition = i % 8;
        
        // If we have a match, set the corresponding bit
        if (_image[i] == maskValue) {
            _mask[byteIndex] |= (1 << bitPosition);
        }
    }
}

  void draw(int x, int y) {
    Locator::getDisplay()->drawRGBBitmap(x, y, _image, _mask.data(), _width, _height); 
  }


};
