#include "Locator.h"

Adafruit_GFX* Locator::_display;
EventBus* Locator::_eventBus;
GFXcanvas16* Locator::_canvas;

void Locator::provide(Adafruit_GFX* display)
{
  _display = display;
}

void Locator::provide(EventBus* eventBus)
{
  _eventBus = eventBus;
}

void Locator::provide(GFXcanvas16* canvas)
{
  _canvas = canvas;
}

Adafruit_GFX* Locator::getDisplay() 
{ 
  return _display; 
}

EventBus* Locator::getEventBus() 
{ 
  return _eventBus; 
}

GFXcanvas16* Locator::getCanvas()
{
  return _canvas;
}
