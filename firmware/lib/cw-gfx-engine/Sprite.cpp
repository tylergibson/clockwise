#include "Sprite.h"
#include <Locator.h>

// Constructors
Sprite::Sprite(uint8_t height, uint8_t width, uint16_t maskValue) : 
    _height(height), _width(width), _length(height*width), _sprites(nullptr), _masks(nullptr), _totalFrames(1), _maskValue(maskValue) {
}

Sprite::Sprite(uint8_t height, uint8_t width, uint16_t maskValue, uint8_t totalFrames) : 
    _height(height), _width(width), _length(height*width), _sprites(nullptr), _masks(nullptr), _totalFrames(totalFrames), _maskValue(maskValue) {
}

Sprite::Sprite(SpriteConfig config) : 
    _height(config.height), _width(config.width), _length(config.length), _sprites(nullptr), _masks(nullptr), _totalFrames(config.total_frames), _maskValue(config.mask_value) {
}

// Destructor
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
        const uint16_t* sprite = _sprites[frame];
        if (!sprite) continue;
        
        // Calculate bytes per row (round up to nearest byte)
        size_t bytesPerRow = (_width + 7) / 8;
        size_t maskSize = bytesPerRow * _height;
        
        // Allocate and zero initialize the mask array
        uint8_t* mask = new uint8_t[maskSize]();
        
        // Create mask with MSB first for each group of 8 pixels
        for (size_t row = 0; row < _height; row++) {
            for (size_t col = 0; col < _width; col++) {
                if (sprite[row * _width + col] != _maskValue) {
                    size_t byteIndex = row * bytesPerRow + (col / 8);
                    size_t bitPosition = 7 - (col % 8);  // MSB first
                    mask[byteIndex] |= (1 << bitPosition);
                }
            }
        }
        
        _masks[frame] = mask;
    }
}

void Sprite::setStaticSprite(const uint16_t* sprite) {
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
    _masks = new const uint8_t*[1];
    
    // Set the single sprite and store mask value
    _sprites[0] = sprite;
    
    _isAnimated = false;
    _totalFrames = 1;
    _currentFrame = 0;
    
    // Create the mask
    createBitMask();
}

void Sprite::setAnimatedSprite(const uint16_t* sprites) {
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
    _sprites = new const uint16_t*[_totalFrames];
    _masks = new const uint8_t*[_totalFrames];
    
    // Split sprites into chunks of size _length and store them
    for (uint8_t i = 0; i < _totalFrames; i++) {
        _sprites[i] = &sprites[i * _length];
    }
    
    _isAnimated = true;
    _currentFrame = 0;
    
    // Create the masks
    createBitMask();
}

const uint16_t* Sprite::getCurrentSprite() const {
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
    // unsigned long current_time = millis();
    
    // // Check if we should process a new tick
    // if (current_time - _last_tick >= _tick_interval) {
    //     _last_tick = current_time;
        
    //     // Check if virtual input duration has expired
    //     if (_virtual_input.duration > 0 && 
    //         current_time - _virtual_input.start_time >= _virtual_input.duration) {
    //         // Reset virtual inputs
    //         _virtual_input.left = false;
    //         _virtual_input.right = false;
    //         _virtual_input.up = false;
    //         _virtual_input.down = false;
    //         _virtual_input.jump = false;
    //         _virtual_input.run_modifier = false;
    //         _virtual_input.duration = 0;
    //     }
        
    //     // Update physics and apply inputs
    //     updatePhysics();
    //     applyInputs();
    //     updatePosition();
    //     checkGroundPlane();
        
    //     // Update sprite position based on physics
    //     _x = static_cast<int8_t>(_position_x);
    //     _y = static_cast<int8_t>(_position_y);
    // }
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

void Sprite::checkGroundPlane() {
    // Ground collision check using configured ground height
    if (_position_y >= _ground_height) {
        _position_y = _ground_height;
        _velocity_y = 0;
        _is_grounded = true;
    }
    
    // Wall collisions only if sprite is on screen
    // if (_position_x <= 0) {
    //     _position_x = 0;
    //     _velocity_x = 0;
    // }
    // if (_position_x >= 63) { // Assuming 64x64 display
    //     _position_x = 63;
    //     _velocity_x = 0;
    // }
}

// Movement and animation methods
void Sprite::startMoving(int targetX, int targetY, unsigned long duration, bool shouldReturnToOrigin) {
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

void Sprite::reverseMoving(int targetX, int targetY) {
    _moveStartTime = millis();
    _moveInitialX = getX();
    _moveInitialY = getY();
    _moveTargetX = targetX;
    _moveTargetY = targetY;
    _shouldReturnToOrigin = false;
    _moving = true;
    _isReversing = true;
}

void Sprite::stopMoving() { _moving = false; }
bool Sprite::isMoving() const { return _moving; }

// Position and dimension methods
void Sprite::setX(int newX) { _x = newX; _position_x = newX; }
void Sprite::setY(int newY) { _y = newY; _position_y = newY; }
void Sprite::setPosition(int newX, int newY) { _x = newX; _y = newY; _position_x = newX; _position_y = newY; }
int Sprite::getX() const { return _x; }
int Sprite::getY() const { return _y; }
uint8_t Sprite::getWidth() const { return _width; }
uint8_t Sprite::getHeight() const { return _height; }
void Sprite::setDimensions(uint8_t width, uint8_t height) { _width = width; _height = height; }

// Linear interpolation helper
int Sprite::lerp(int start, int end, float t) {
    return static_cast<int>(start + (end - start) * t);
}

// Getters for movement state
bool Sprite::shouldReturnToOrigin() const { return _shouldReturnToOrigin; }
float Sprite::getVelocityX() const { return _velocity_x; }
float Sprite::getVelocityY() const { return _velocity_y; }
bool Sprite::isGrounded() const { return _is_grounded; }

// Animation management
void Sprite::addAnimation(const std::string& name, uint8_t startFrame, uint8_t endFrame) {
    Animation anim;
    anim.name = name;
    anim.startFrame = startFrame;
    anim.endFrame = endFrame;
    anim.index = _animations.size();
    _animations.push_back(anim);
}

void Sprite::playAnimation(uint8_t index) {
    if (index < _animations.size()) {
        _currentAnimation = index;
        _currentFrame = _animations[index].startFrame;
        _isPlaying = true;
    }
}

void Sprite::playAnimation(const std::string& name) {
    for (size_t i = 0; i < _animations.size(); i++) {
        if (_animations[i].name == name) {
            playAnimation(i);
            return;
        }
    }
}

void Sprite::stopAnimation() { _isPlaying = false; }
bool Sprite::isPlaying() const { return _isPlaying; }

void Sprite::nextFrame() {
    if (!_isPlaying || !_isAnimated) return;
    
    _currentFrame++;
    if (_currentFrame > _animations[_currentAnimation].endFrame) {
        _currentFrame = _animations[_currentAnimation].startFrame;
    }
}

const Animation* Sprite::getCurrentAnimation() const {
    return _currentAnimation < _animations.size() ? &_animations[_currentAnimation] : nullptr;
}

// Collision detection
boolean Sprite::collidedWith(Sprite* sprite) {
    if (!sprite) return false;
    
    int thisRight = _x + _width;
    int thisBottom = _y + _height;
    int otherRight = sprite->getX() + sprite->getWidth();
    int otherBottom = sprite->getY() + sprite->getHeight();
    
    return !(_x >= otherRight || thisRight <= sprite->getX() ||
             _y >= otherBottom || thisBottom <= sprite->getY());
}

void Sprite::logPosition() {
    Serial.print("Sprite position: (");
    Serial.print(_x);
    Serial.print(", ");
    Serial.print(_y);
    Serial.println(")");
}

void Sprite::checkCollision(std::shared_ptr<Sprite>& other) {
    // Get bounding boxes
    int16_t x1 = getX();
    int16_t y1 = getY();
    int16_t w1 = getWidth();
    int16_t h1 = getHeight();
    
    int16_t x2 = other->getX();
    int16_t y2 = other->getY();
    int16_t w2 = other->getWidth();
    int16_t h2 = other->getHeight();
    
    // Check for overlap
    if (!(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)) {
        handleCollision(other);
    }
}

void Sprite::handleCollision(std::shared_ptr<Sprite>& other) {
    // Calculate combined velocity vector
    float vx1 = getVelocityX();
    float vy1 = getVelocityY();
    float vx2 = other->getVelocityX();
    float vy2 = other->getVelocityY();
    
    float combinedVx = vx1 + vx2;
    float combinedVy = vy1 + vy2;
    
    // Find closest cardinal direction
    float magnitude = sqrt(combinedVx * combinedVx + combinedVy * combinedVy);
    float angle = atan2(combinedVy, combinedVx);
    
    // Convert angle to nearest cardinal direction
    Direction direction;
    if (angle >= -M_PI/4 && angle < M_PI/4) {
        direction = RIGHT;
    } else if (angle >= M_PI/4 && angle < 3*M_PI/4) {
        direction = DOWN;
    } else if (angle >= 3*M_PI/4 || angle < -3*M_PI/4) {
        direction = LEFT;
    } else {
        direction = UP;
    }
    
    // Apply collision response
    collided(direction);
    other->collided(direction);
}

void Sprite::collided(Direction direction) {
    // Apply the collision force based on direction
}
