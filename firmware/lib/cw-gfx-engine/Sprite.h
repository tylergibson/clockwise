#pragma once

#include <Arduino.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <limits.h>

// Each asset is defined as an array of 16-bit unsigned integers.
// The config array defines the height, width, length, and mask value for the asset.
//  For animated assets, the config array also includes the number of frames in the last position.
// The data array defines the asset's pixels.
//  For animated assets, the data array is a 2D array of the frames.
struct SpriteConfig {
    const uint8_t height;
    const uint8_t width;
    const uint16_t length;
    const uint16_t mask_value;
    const uint8_t total_frames=1;

		SpriteConfig(const uint8_t height, const uint8_t width, const uint16_t mask_value, const uint8_t total_frames) : 
			height(height), width(width), length(height*width), mask_value(mask_value), total_frames(total_frames) {}
		SpriteConfig(const uint8_t height, const uint8_t width, const uint16_t mask_value) : 
			height(height), width(width), length(height*width), mask_value(mask_value), total_frames(1) {}
};

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

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Animation structure
struct Animation {
    uint8_t index;
    std::string name;
    uint8_t startFrame;
    uint8_t endFrame;
};

class Sprite {
  protected:
    // Sprites always start located at the minimum possible position (hopefully off screen)
    int _x = INT_MIN;
    int _y = INT_MIN;
    uint8_t _height;
    uint8_t _width;
    uint16_t _length;
    const uint16_t** _sprites = nullptr;  // Array of sprite pointers
    const uint8_t** _masks = nullptr;    // Array of mask pointers
    uint16_t _maskValue = 0;        // Value to use for creating masks
    bool _visible = true;
    bool _isAnimated = false;
    
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
    float _position_x = INT_MIN;
    float _position_y = INT_MIN;
    bool _is_grounded = true;
    
    // Ground configuration
    float _ground_height = 0; // Default to bottom of display
    
    // Animation state
    std::vector<Animation> _animations;
    uint8_t _currentAnimation = 0;
    bool _isPlaying = false;
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
    void createBitMask();

    // Helper function for clamping values
    template<typename T>
    T clamp(T value, T min, T max) {
        return std::min(std::max(value, min), max);
    }

    // Collision handling
    void handleCollision(std::shared_ptr<Sprite>& other);

  public:
    void checkGroundPlane();
    void collided(Direction direction);
    Sprite(uint8_t height, uint8_t width, uint16_t maskValue);
    Sprite(uint8_t height, uint8_t width, uint16_t maskValue, uint8_t totalFrames);
    Sprite(SpriteConfig config);

    virtual ~Sprite();
    
    // Sprite and mask management
    void setStaticSprite(const uint16_t* sprite);
    void setAnimatedSprite(const uint16_t* sprites);
    const uint16_t* getCurrentSprite() const;
    const uint8_t* getCurrentMask() const;
    bool isAnimated() const { return _isAnimated; }
    
    // Virtual input control functions
    void setVirtualInput(bool left, bool right, bool up, bool down, bool jump, bool run_modifier, unsigned long duration_ms);
    void setTickInterval(unsigned long interval_ms);
    void setGroundHeight(float height);
    float getGroundHeight() const;
    
    // Movement and animation methods
    void startMoving(int targetX, int targetY, unsigned long duration, bool shouldReturnToOrigin);
    void reverseMoving(int targetX, int targetY);
    void stopMoving();
    bool isMoving() const;
    
    // Position and dimension methods
    void setX(int newX);
    void setY(int newY);
    void setPosition(int newX, int newY);
    int getX() const;
    int getY() const;
    uint8_t getWidth() const;
    uint8_t getHeight() const;
    void setDimensions(uint8_t height, uint8_t width);
    
    // Linear interpolation helper
    int lerp(int start, int end, float t);
    
    // Update method to be called in game loop
    virtual void update();
    
    // Getters for movement state
    bool shouldReturnToOrigin() const;
    float getVelocityX() const;
    float getVelocityY() const;
    bool isGrounded() const;

    boolean collidedWith(Sprite* sprite);
    void logPosition();

    // Animation management
    void addAnimation(const std::string& name, uint8_t startFrame, uint8_t endFrame);
    void playAnimation(uint8_t index);
    void playAnimation(const std::string& name);
    void stopAnimation();
    bool isPlaying() const;
    void nextFrame();
    const Animation* getCurrentAnimation() const;

    //virtual const char* name();

    void checkCollision(std::shared_ptr<Sprite>& other);
};
