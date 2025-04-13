#include "Sprite.h"
#include <Locator.h>

Sprite::Sprite(int8_t x, int8_t y) : _x(x), _y(y), _position_x(x), _position_y(y), 
    _sprites(nullptr), _masks(nullptr), _totalFrames(0) {}

Sprite::~Sprite() {
    // Clean up arrays if they were allocated
    if (_sprites) {
        delete[] _sprites;
    }
    if (_masks) {
        for (uint8_t i = 0; i < _totalFrames; i++) {
            delete[] _masks[i];
        }
        delete[] _masks;
    }
}

void Sprite::createBitMask() {
    if (!_sprites || !_masks) return;
    
    for (uint8_t frame = 0; frame < _totalFrames; frame++) {
        const unsigned short* sprite = _sprites[frame];
        if (!sprite) continue;
        
        // Calculate the size needed for the mask array
        size_t maskSize = (_width * _height + 7) / 8;
        
        // Allocate memory for the mask
        uint8_t* mask = new uint8_t[maskSize];
        std::fill(mask, mask + maskSize, 0);
        
        for (size_t i = 0; i < _width * _height; ++i) {
            // Calculate which byte and bit position we're working with
            size_t byteIndex = i / 8;
            size_t bitPosition = i % 8;
            
            // If we have a match, set the corresponding bit
            if (sprite[i] == _maskValue) {
                mask[byteIndex] |= (1 << bitPosition);
            }
        }
        
        _masks[frame] = mask;
    }
}

void Sprite::setStaticSprite(const unsigned short* sprite, unsigned short maskValue) {
    // Clean up existing arrays if any
    if (_sprites) {
        delete[] _sprites;
    }
    if (_masks) {
        for (uint8_t i = 0; i < _totalFrames; i++) {
            delete[] _masks[i];
        }
        delete[] _masks;
    }
    
    // Allocate single-item arrays
    _sprites = new const unsigned short*[1];
    _masks = new uint8_t*[1];
    
    // Set the single sprite and store mask value
    _sprites[0] = sprite;
    _maskValue = maskValue;
    
    _isAnimated = false;
    _totalFrames = 1;
    _currentFrame = 0;
    
    // Create the mask
    createBitMask();
}

void Sprite::setAnimatedSprite(const unsigned short** sprites, unsigned short maskValue, uint8_t totalFrames) {
    // Clean up existing arrays if any
    if (_sprites) {
        delete[] _sprites;
    }
    if (_masks) {
        for (uint8_t i = 0; i < _totalFrames; i++) {
            delete[] _masks[i];
        }
        delete[] _masks;
    }
    
    // Allocate new arrays
    _sprites = new const unsigned short*[totalFrames];
    _masks = new uint8_t*[totalFrames];
    
    // Copy the sprite pointers and store mask value
    for (uint8_t i = 0; i < totalFrames; i++) {
        _sprites[i] = sprites[i];
    }
    _maskValue = maskValue;
    
    _isAnimated = true;
    _totalFrames = totalFrames;
    _currentFrame = 0;
    
    // Create the masks
    createBitMask();
}

const unsigned short* Sprite::getCurrentSprite() const {
    return _sprites ? _sprites[_currentFrame] : nullptr;
}

const uint8_t* Sprite::getCurrentMask() const {
    return _masks ? _masks[_currentFrame] : nullptr;
}

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

void Sprite::addAnimation(const std::string& name, uint8_t startFrame, uint8_t endFrame) {
    if (startFrame >= _totalFrames || endFrame >= _totalFrames || startFrame > endFrame) {
        return; // Invalid frame range
    }
    
    Animation anim;
    anim.index = _animations.size();
    anim.name = name;
    anim.startFrame = startFrame;
    anim.endFrame = endFrame;
    _animations.push_back(anim);
}

void Sprite::playAnimation(uint8_t index) {
    if (index >= _animations.size()) {
        return;
    }
    _currentAnimation = index;
    _currentFrame = _animations[index].startFrame;
    _isPlaying = true;
}

void Sprite::playAnimation(const std::string& name) {
    for (const auto& anim : _animations) {
        if (anim.name == name) {
            playAnimation(anim.index);
            return;
        }
    }
}

void Sprite::stopAnimation() {
    _isPlaying = false;
}

bool Sprite::isPlaying() const {
    return _isPlaying;
}

const Animation* Sprite::getCurrentAnimation() const {
    if (_currentAnimation >= _animations.size()) {
        return nullptr;
    }
    return &_animations[_currentAnimation];
}

void Sprite::nextFrame() {
    if (!_isPlaying) return;
    
    const Animation* anim = getCurrentAnimation();
    if (anim) {
        _currentFrame++;
        if (_currentFrame > anim->endFrame) {
            _currentFrame = anim->startFrame;
        }
    }
}
