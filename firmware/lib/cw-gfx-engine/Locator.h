#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "EventBus.h"

class Locator {
  private: 
    static Adafruit_GFX* _display;
    static EventBus* _eventBus;
    static GFXcanvas16* _canvas;
    static constexpr uint16_t CANVAS_WIDTH = 196;
    static constexpr uint16_t CANVAS_HEIGHT = 64;
    static constexpr uint16_t DISPLAY_WIDTH = 64;
    static constexpr uint16_t DISPLAY_HEIGHT = 64;
    static constexpr uint16_t CANVAS_OFFSET_X = (CANVAS_WIDTH - DISPLAY_WIDTH) / 2;

  public:    
    static Adafruit_GFX* getDisplay();
    static EventBus* getEventBus();
    static GFXcanvas16* getCanvas();
    static void provide(Adafruit_GFX* display);
    static void provide(EventBus* eventBus);
    static void renderToDisplay();
    static void clearCanvas();
};
