#include "Locator.h"

Adafruit_GFX* Locator::_display;
EventBus* Locator::_eventBus;
GFXcanvas16* Locator::_canvas = new GFXcanvas16(CANVAS_WIDTH, CANVAS_HEIGHT);

void Locator::provide(Adafruit_GFX* display)
{
  _display = display;
}

void Locator::provide(EventBus* eventBus)
{
  _eventBus = eventBus;
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

void Locator::renderToDisplay()
{
  if (_display && _canvas) {
    _display->drawRGBBitmap(0, 0, _canvas->getBuffer() + (CANVAS_OFFSET_X * CANVAS_HEIGHT), DISPLAY_WIDTH, DISPLAY_HEIGHT);
  }
}

void Locator::clearCanvas()
{
  if (_canvas) {
    _canvas->fillScreen(0);
  }
}
