#pragma once
#include <vector>
#include <cstdint>
#include "Object.h"

class AnimatedObject {
    private:
      size_t _x;
      size_t _y;
      size_t _height;
      size_t _width;
      size_t _num_frames;
      size_t _current_frame;
      std::vector<Object> _frames;
    public:
      AnimatedObject(const unsigned short *rawFrames, size_t x, size_t y, short *config) {
        _height = config[0];
        _width = config[1];
        _num_frames = config[4];
        _x = x;
        _y = y;
        _current_frame = 0;
        _frames.resize(this->_num_frames);
        for(size_t i = 0; i < this->_num_frames; i++) {
            _frames[i] = Object(&rawFrames[i],config);
        }
        
      }

    void advanceFrame(size_t interval=1) {
        if(_num_frames > 1) {
            _current_frame = (_current_frame + interval) % _num_frames;
        }
    }

    void setPosition(size_t x, size_t y) {
        _x = x;
        _y = y;
    }

    void drawCurrentFrame() {
        if (_current_frame < _frames.size()) {
            _frames[_current_frame].draw(_x, _y);
        }
    }
  };