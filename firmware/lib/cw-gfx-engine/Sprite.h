#pragma once

#include <Arduino.h>
#include <vector>
#include <memory>
#include <algorithm>

// Virtual input state structure
struct VirtualInput {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool jump = false;
    bool run_modifier = false;
    unsigned long start_time = 0;
    unsigned long duration = 0;
};

class Sprite {
  protected:
    int8_t _x;
    int8_t _y;
    uint8_t _width;
    uint8_t _height;
    const unsigned short* _sprite;
    const unsigned short* _mask;
    bool _visible = true;
    
    // Virtual input state
    VirtualInput _virtual_input;
    unsigned long _last_tick = 0;
    unsigned long _tick_interval = 16; // ~60fps default
    
    // Physics constants
    static constexpr float ACCELERATION = 0.5f;
    static constexpr float FRICTION = 0.2f;
    static constexpr float MAX_SPEED = 5.0f;
    static constexpr float JUMP_FORCE = -8.0f;
    static constexpr float GRAVITY = 0.4f;
    
    // Physics state
    float _velocity_x = 0;
    float _velocity_y = 0;
    float _position_x = 0;
    float _position_y = 0;
    bool _is_grounded = true;
    
    // Ground configuration
    float _ground_height = 63; // Default to bottom of display
    
    // Animation state
    uint8_t _totalFrames = 0;
    uint8_t _currentFrame = 0;
    uint8_t _spriteReference = 0;
    uint8_t _currentFrameCount = 0;
    unsigned long _lastMillisSpriteFrames = 0;
    unsigned long _lastResetTime = 0;
    unsigned long _lastResetMoveTime = 0;
    bool _moving = false;
    unsigned long _moveStartTime = 1;
    unsigned long _moveDuration = 0;
    int8_t _moveInitialX = 0;
    int8_t _moveInitialY = 0;
    int8_t _moveTargetX = -1;
    int8_t _moveTargetY = -1;
    bool _shouldReturnToOrigin = false;
    bool _isReversing = false;

    // Physics update methods
    void updatePhysics();
    void applyInputs();
    void updatePosition();
    void checkCollisions();

    // Helper function for clamping values
    template<typename T>
    T clamp(T value, T min, T max) {
        return std::min(std::max(value, min), max);
    }

  public:
    Sprite(int8_t x, int8_t y);
    virtual ~Sprite() = default;
    
    // Virtual input control functions
    void setVirtualInput(bool left, bool right, bool up, bool down, bool jump, bool run_modifier, unsigned long duration_ms);
    void setTickInterval(unsigned long interval_ms);
    void setGroundHeight(float height);
    float getGroundHeight() const;
    
    // Movement and animation methods
    void startMoving(int8_t targetX, int8_t targetY, unsigned long duration, bool shouldReturnToOrigin);
    void reverseMoving(int8_t targetX, int8_t targetY);
    void stopMoving();
    bool isMoving() const;
    void incFrame();
    
    // Position and dimension methods
    void setX(int8_t newX);
    void setY(int8_t newY);
    int8_t getX() const;
    int8_t getY() const;
    uint8_t getWidth() const;
    uint8_t getHeight() const;
    void setDimensions(uint8_t width, uint8_t height);
    
    // Linear interpolation helper
    int8_t lerp(int8_t start, int8_t end, float t);
    
    // Update method to be called in game loop
    virtual void update();
    
    // Getters for movement state
    bool shouldReturnToOrigin() const;
    float getVelocityX() const;
    float getVelocityY() const;
    bool isGrounded() const;

    boolean collidedWith(Sprite* sprite);
    void logPosition();

    virtual const char* name();
};
