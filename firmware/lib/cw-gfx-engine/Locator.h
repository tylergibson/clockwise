#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "EventBus.h"

class Locator {
  private: 
    static Adafruit_GFX* _display;
    static EventBus* _eventBus;
    static GFXcanvas16* _canvas;

  public:    
    static Adafruit_GFX* getDisplay();
    static EventBus* getEventBus();
    static GFXcanvas16* getCanvas();
    static void provide(Adafruit_GFX* display);
    static void provide(EventBus* eventBus);
    static void provide(GFXcanvas16* canvas);
};
