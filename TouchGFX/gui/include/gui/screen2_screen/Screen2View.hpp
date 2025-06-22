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
protected:
    touchgfx::Unicode::UnicodeChar timeCountBuffer[3];
    // Game constants
    static const int GROUND_LEVEL = 146;
    static const int LEFT_BOUND = 0;
    static const int RIGHT_BOUND = 320;
    static const int SCREEN_CENTER = 160;
    
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
    
    // Movement speeds
    static const int MOVE_SPEED = 5;
    static const float JUMP_VELOCITY;
    static const float GRAVITY;
    static const uint16_t JUMP_THRESHOLD = 500;  // Sensitivity for jump trigger (lower = more sensitive)
};

#endif // SCREEN2VIEW_HPP
