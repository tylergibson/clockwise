#include "Sprite.h"
#include <Locator.h>

Sprite::Sprite(int8_t x, int8_t y) : _x(x), _y(y), _position_x(x), _position_y(y) {}

void Sprite::setVirtualInput(bool left, bool right, bool up, bool down, bool jump, bool run_modifier, unsigned long duration_ms) {
    _virtual_input.left = left;
    _virtual_input.right = right;
    _virtual_input.up = up;
    _virtual_input.down = down;
    _virtual_input.jump = jump;
    _virtual_input.run_modifier = run_modifier;
    _virtual_input.start_time = millis();
    _virtual_input.duration = duration_ms;
}

void Sprite::setTickInterval(unsigned long interval_ms) {
    _tick_interval = interval_ms;
}

void Sprite::setGroundHeight(float height) {
    _ground_height = height;
}

float Sprite::getGroundHeight() const {
    return _ground_height;
}

void Sprite::update() {
    unsigned long current_time = millis();
    
    // Check if we should process a new tick
    if (current_time - _last_tick >= _tick_interval) {
        _last_tick = current_time;
        
        // Check if virtual input duration has expired
        if (_virtual_input.duration > 0 && 
            current_time - _virtual_input.start_time >= _virtual_input.duration) {
            // Reset virtual inputs
            _virtual_input.left = false;
            _virtual_input.right = false;
            _virtual_input.up = false;
            _virtual_input.down = false;
            _virtual_input.jump = false;
            _virtual_input.run_modifier = false;
            _virtual_input.duration = 0;
        }
        
        // Update physics and apply inputs
        updatePhysics();
        applyInputs();
        updatePosition();
        checkCollisions();
        
        // Update sprite position based on physics
        _x = static_cast<int8_t>(_position_x);
        _y = static_cast<int8_t>(_position_y);
    }
}

void Sprite::updatePhysics() {
    // Apply gravity if not grounded
    if (!_is_grounded) {
        _velocity_y += GRAVITY;
    }
    
    // Apply friction
    if (_velocity_x > 0) {
        _velocity_x = std::max(0.0f, _velocity_x - FRICTION);
    } else if (_velocity_x < 0) {
        _velocity_x = std::min(0.0f, _velocity_x + FRICTION);
    }
    
    // Cap velocities
    _velocity_x = clamp(_velocity_x, -MAX_SPEED, MAX_SPEED);
    _velocity_y = clamp(_velocity_y, -MAX_SPEED, MAX_SPEED);
}

void Sprite::applyInputs() {
    float accel = ACCELERATION;
    if (_virtual_input.run_modifier) {
        accel *= 2.0f; // Double acceleration when running
    }
    
    // Apply horizontal movement
    if (_virtual_input.left) {
        _velocity_x = std::max(_velocity_x - accel, -MAX_SPEED);
    }
    if (_virtual_input.right) {
        _velocity_x = std::min(_velocity_x + accel, MAX_SPEED);
    }
    
    // Apply vertical movement
    if (_virtual_input.up) {
        _velocity_y = std::max(_velocity_y - accel, -MAX_SPEED);
    }
    if (_virtual_input.down) {
        _velocity_y = std::min(_velocity_y + accel, MAX_SPEED);
    }
    
    // Apply jump
    if (_virtual_input.jump && _is_grounded) {
        _velocity_y = JUMP_FORCE;
        _is_grounded = false;
    }
}

void Sprite::updatePosition() {
    _position_x += _velocity_x;
    _position_y += _velocity_y;
    
    // Only clamp X position to keep sprite visible on display
    _position_x = clamp(_position_x, 0.0f, 63.0f); // Assuming 64x64 display
}

void Sprite::checkCollisions() {
    // Ground collision check using configured ground height
    if (_position_y >= _ground_height) {
        _position_y = _ground_height;
        _velocity_y = 0;
        _is_grounded = true;
    }
    
    // Wall collisions only if sprite is on screen
    if (_position_x <= 0) {
        _position_x = 0;
        _velocity_x = 0;
    }
    if (_position_x >= 63) { // Assuming 64x64 display
        _position_x = 63;
        _velocity_x = 0;
    }
}

// Movement and animation methods
void Sprite::startMoving(int8_t targetX, int8_t targetY, unsigned long duration, bool shouldReturnToOrigin) {
    _moveStartTime = millis();
    _moveDuration = duration;
    _moveInitialX = getX();
    _moveInitialY = getY();
    _moveTargetX = targetX;
    _moveTargetY = targetY;
    _shouldReturnToOrigin = shouldReturnToOrigin;
    _moving = true;
    _isReversing = false;
}

void Sprite::reverseMoving(int8_t targetX, int8_t targetY) {
    _moveStartTime = millis();
    _moveDuration = _moveDuration;
    _moveInitialX = getX();
    _moveInitialY = getY();
    _moveTargetX = targetX;
    _moveTargetY = targetY;
    _shouldReturnToOrigin = false;
    _moving = true;
    _isReversing = true;
}

void Sprite::stopMoving() {
    _moving = false;
}

bool Sprite::isMoving() const {
    return _moving;
}

void Sprite::incFrame() {
    if (_totalFrames > 1) {
        _currentFrame = (_currentFrame + 1) % _totalFrames;
    }
}

// Position and dimension methods
void Sprite::setX(int8_t newX) {
    _x = newX;
    _position_x = newX;
}

void Sprite::setY(int8_t newY) {
    _y = newY;
    _position_y = newY;
}

int8_t Sprite::getX() const {
    return _x;
}

int8_t Sprite::getY() const {
    return _y;
}

uint8_t Sprite::getWidth() const {
    return _width;
}

uint8_t Sprite::getHeight() const {
    return _height;
}

void Sprite::setDimensions(uint8_t width, uint8_t height) {
    _width = width;
    _height = height;
}

// Linear interpolation helper
int8_t Sprite::lerp(int8_t start, int8_t end, float t) {
    return start + static_cast<int8_t>(t * (end - start));
}

// Getters for movement state
bool Sprite::shouldReturnToOrigin() const {
    return _shouldReturnToOrigin;
}

float Sprite::getVelocityX() const {
    return _velocity_x;
}

float Sprite::getVelocityY() const {
    return _velocity_y;
}

bool Sprite::isGrounded() const {
    return _is_grounded;
}

// Existing collision detection method
boolean Sprite::collidedWith(Sprite* sprite) {
    return !(_x + _width < sprite->_x || 
             _x > sprite->_x + sprite->_width || 
             _y + _height < sprite->_y || 
             _y > sprite->_y + sprite->_height);
}

void Sprite::logPosition() {
    Serial.print("X: ");
    Serial.print(_x);
    Serial.print(" Y: ");
    Serial.println(_y);
}
