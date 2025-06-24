#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    
    // Method to handle joystick data from queue
    void handleJoystickData();
    
    // Player action methods
    void playerLeftJump();
    void playerLeftMoveForward();
    void playerLeftMoveBackward();
    void playerRightJump();
    void playerRightMoveForward();
    void playerRightMoveBackward();
    
    // Physics update method
    void updatePhysics();

    //Display Time
    void displayCounter(int newCount);
    
    // Game management
    void resetGame();
    void updateScoreDisplay();
    
    // Ball and goal detection
    void checkGoalDetection();
    void resetBallPosition();
    void resetAfterGoal();
    
    // Ball physics and collision
    void updateBallPhysics();
    void checkPlayerBallCollision();
    void shoot(float velocityX, float velocityY);
    bool isColliding(int obj1X, int obj1Y, int obj1Width, int obj1Height,
                     int obj2X, int obj2Y, int obj2Width, int obj2Height);
    
protected:
    touchgfx::Unicode::UnicodeChar timeCountBuffer[3];
    touchgfx::Unicode::UnicodeChar scoreLeftBuffer[3];
    touchgfx::Unicode::UnicodeChar scoreRightBuffer[3];
    
    // Game constants
    static const int GROUND_LEVEL = 146;
    static const int LEFT_BOUND = 0;
    static const int RIGHT_BOUND = 320;
    static const int SCREEN_CENTER = 160;
    
    // Goal boundaries
    static const int LEFT_GOAL_MIN_X = 0;
    static const int LEFT_GOAL_MAX_X = 20;
    static const int RIGHT_GOAL_MIN_X = 300;
    static const int RIGHT_GOAL_MAX_X = 320;
    static const int GOAL_MIN_Y = 85;
    static const int GOAL_MAX_Y = 200;  // Ball needs to be above 130 (Y <= 130)
    
    // Ball position and initial position
    int ballX, ballY;
    static const int BALL_INITIAL_X = 153;
    static const int BALL_INITIAL_Y = 108;
    static const int BALL_SIZE = 15;
    
    // Ball physics
    float ballVelocityX, ballVelocityY;
    static constexpr float BALL_FRICTION = 0.98f;
    static constexpr float BALL_BOUNCE_DAMPING = 0.7f;
    static constexpr float BALL_MIN_VELOCITY = 0.5f;
    static constexpr float BALL_GRAVITY = 0.4f;  // Gravity force applied to ball each frame
    
    // Player positions
    int playerLeftX, playerLeftY;
    int playerRightX, playerRightY;
    
    // Previous player positions for proper invalidation
    int prevPlayerLeftX, prevPlayerLeftY;
    int prevPlayerRightX, prevPlayerRightY;
    
    // Physics variables for jumping
    float playerLeftVelocityY, playerRightVelocityY;
    bool playerLeftOnGround, playerRightOnGround;
    bool playerLeftJumping, playerRightJumping;
    
    // Previous joystick states for edge detection (only for jumping)
    bool prevJ1UpPressed, prevJ2UpPressed;
    
    // Goal detection cooldown to prevent multiple rapid detections
    int goalCooldown;
    static const int GOAL_COOLDOWN_FRAMES = 120; // 2 seconds at 60fps
    
    // Movement speeds
    static const int MOVE_SPEED = 5;
    static const float JUMP_VELOCITY;
    static const float GRAVITY;
    static const uint16_t JUMP_THRESHOLD = 500;  // Sensitivity for jump trigger (lower = more sensitive)
    
    // Player and ball dimensions for collision detection
    static const int PLAYER_LEFT_WIDTH = 40;
    static const int PLAYER_LEFT_HEIGHT = 54;
    static const int PLAYER_RIGHT_WIDTH = 30;
    static const int PLAYER_RIGHT_HEIGHT = 54;
};

#endif // SCREEN2VIEW_HPP
