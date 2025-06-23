#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/model/Model.hpp>
#include "cmsis_os.h"
#include "joystick_data.h"
#include <cmath>

// Extern declaration for the joystick data queue
extern osMessageQueueId_t joystickDataQueue;

// Initialize static constants
const float Screen2View::JUMP_VELOCITY = -15.0f; // Negative for upward movement
const float Screen2View::GRAVITY = 0.8f;

Screen2View::Screen2View()
{
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    
    // Initialize counter display with starting value
    displayCounter(90);
    
    // Initialize score displays
    updateScoreDisplay();
    
    // Initialize player positions based on their initial setup
    // From Screen2ViewBase.cpp: playerLeft is at (55, 146), playerRight is at (236, 146)
    playerLeftX = 55;
    playerLeftY = GROUND_LEVEL;
    playerRightX = 236;
    playerRightY = GROUND_LEVEL;

    // Initialize previous positions
    prevPlayerLeftX = playerLeftX;
    prevPlayerLeftY = playerLeftY;
    prevPlayerRightX = playerRightX;
    prevPlayerRightY = playerRightY;

    // Initialize physics variables
    playerLeftVelocityY = 0.0f;
    playerRightVelocityY = 0.0f;
    playerLeftOnGround = true;
    playerRightOnGround = true;
    playerLeftJumping = false;
    playerRightJumping = false;

    // Initialize previous joystick states (only for jumping)
    prevJ1UpPressed = false;
    prevJ2UpPressed = false;
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::handleTickEvent()
{
    // Call parent handleTickEvent first
    Screen2ViewBase::handleTickEvent();

    // Store previous positions BEFORE any movement happens
    prevPlayerLeftX = playerLeftX;
    prevPlayerLeftY = playerLeftY;
    prevPlayerRightX = playerRightX;
    prevPlayerRightY = playerRightY;

    // Handle joystick input each tick (this may change player positions)
    handleJoystickData();

    // Update physics (this may also change player positions)
    updatePhysics();
}

void Screen2View::updatePhysics()
{
    // Update playerLeft physics
    if (playerLeftJumping || !playerLeftOnGround)
    {
        playerLeftVelocityY += GRAVITY;
        playerLeftY += (int)playerLeftVelocityY;

        // Check ground collision
        if (playerLeftY >= GROUND_LEVEL)
        {
            playerLeftY = GROUND_LEVEL;
            playerLeftVelocityY = 0.0f;
            playerLeftOnGround = true;
            playerLeftJumping = false;
        }
        else
        {
            playerLeftOnGround = false;
        }
    }

    // Update playerRight physics
    if (playerRightJumping || !playerRightOnGround)
    {
        playerRightVelocityY += GRAVITY;
        playerRightY += (int)playerRightVelocityY;

        // Check ground collision
        if (playerRightY >= GROUND_LEVEL)
        {
            playerRightY = GROUND_LEVEL;
            playerRightVelocityY = 0.0f;
            playerRightOnGround = true;
            playerRightJumping = false;
        }
        else
        {
            playerRightOnGround = false;
        }
    }

    // Only update positions and invalidate if there's actual movement
    bool leftPlayerMoved = (playerLeftX != prevPlayerLeftX || playerLeftY != prevPlayerLeftY);
    bool rightPlayerMoved = (playerRightX != prevPlayerRightX || playerRightY != prevPlayerRightY);

    if (leftPlayerMoved)
    {
        // Invalidate previous position area (larger area to clear artifacts)
        touchgfx::Rect prevRect(prevPlayerLeftX - 5, prevPlayerLeftY - 5, 50, 64);
        invalidateRect(prevRect);

        // Update player position
        playerLeft.setXY(playerLeftX, playerLeftY);

        // Invalidate new position area
        touchgfx::Rect newRect(playerLeftX - 5, playerLeftY - 5, 50, 64);
        invalidateRect(newRect);
    }

    if (rightPlayerMoved)
    {
        // Invalidate previous position area (larger area to clear artifacts)
        touchgfx::Rect prevRect(prevPlayerRightX - 5, prevPlayerRightY - 5, 40, 64);
        invalidateRect(prevRect);

        // Update player position
        playerRight.setXY(playerRightX, playerRightY);

        // Invalidate new position area
        touchgfx::Rect newRect(playerRightX - 5, playerRightY - 5, 40, 64);
        invalidateRect(newRect);
    }
}

void Screen2View::handleJoystickData()
{
    JoystickData_t joystick_data;

    // Try to get joystick data from queue (non-blocking)
    osStatus_t status = osMessageQueueGet(joystickDataQueue, &joystick_data, NULL, 0);

    if (status == osOK)
    {
        // Successfully received joystick data

        // Define movement parameters
        const uint16_t CENTER_VALUE = 2048; // Center of 12-bit ADC
        const uint16_t DEAD_ZONE = 600;     // Increased dead zone to prevent drift
        const uint16_t Y_DEAD_ZONE = 400;   // Dead zone for Y-axis (jumping)
        const int MOVEMENT_SPEED = 3;       // Continuous movement speed

        // Process Joystick 1 for playerLeft
        // X-axis movement (continuous when held) - with proper dead zone
        if (joystick_data.j1_x > (CENTER_VALUE + DEAD_ZONE))
        {
            // Move right - only if significantly above center
            playerLeftX += MOVEMENT_SPEED;
            if (playerLeftX > RIGHT_BOUND - 40)
            { // Account for player width
                playerLeftX = RIGHT_BOUND - 40;
            }
        }
        else if (joystick_data.j1_x < (CENTER_VALUE - DEAD_ZONE))
        {
            // Move left - only if significantly below center
            playerLeftX -= MOVEMENT_SPEED;
            if (playerLeftX < LEFT_BOUND)
            {
                playerLeftX = LEFT_BOUND;
            }
        }
        // If joystick is in dead zone, no movement occurs

        // Y-axis movement for jumping - improved logic with dead zone
        bool j1_in_y_deadzone = (joystick_data.j1_y >= (CENTER_VALUE - Y_DEAD_ZONE)) &&
                                (joystick_data.j1_y <= (CENTER_VALUE + Y_DEAD_ZONE));
        bool j1_up_pressed = !j1_in_y_deadzone && (joystick_data.j1_y < (CENTER_VALUE - JUMP_THRESHOLD));

        if (j1_up_pressed && !prevJ1UpPressed)
        {
            playerLeftJump(); // Jump on edge detection
        }

        // Process Joystick 2 for playerRight
        // X-axis movement (continuous when held) - with proper dead zone
        if (joystick_data.j2_x > (CENTER_VALUE + DEAD_ZONE))
        {
            // Move right - only if significantly above center
            playerRightX += MOVEMENT_SPEED;
            if (playerRightX > RIGHT_BOUND - 30)
            { // Account for player width
                playerRightX = RIGHT_BOUND - 30;
            }
        }
        else if (joystick_data.j2_x < (CENTER_VALUE - DEAD_ZONE))
        {
            // Move left - only if significantly below center
            playerRightX -= MOVEMENT_SPEED;
            if (playerRightX < LEFT_BOUND)
            {
                playerRightX = LEFT_BOUND;
            }
        }
        // If joystick is in dead zone, no movement occurs

        // Y-axis movement for jumping - improved logic with dead zone
        bool j2_in_y_deadzone = (joystick_data.j2_y >= (CENTER_VALUE - Y_DEAD_ZONE)) &&
                                (joystick_data.j2_y <= (CENTER_VALUE + Y_DEAD_ZONE));
        bool j2_up_pressed = !j2_in_y_deadzone && (joystick_data.j2_y < (CENTER_VALUE - JUMP_THRESHOLD));

        if (j2_up_pressed && !prevJ2UpPressed)
        {
            playerRightJump(); // Jump on edge detection
        }

        // Update previous states only for jumping (to maintain edge detection for jumps)
        prevJ1UpPressed = j1_up_pressed;
        prevJ2UpPressed = j2_up_pressed;
    }
}

// Player Left Action Functions
void Screen2View::playerLeftJump()
{
    if (playerLeftOnGround)
    {
        playerLeftVelocityY = JUMP_VELOCITY;
        playerLeftOnGround = false;
        playerLeftJumping = true;
    }
}

void Screen2View::playerLeftMoveForward()
{
    playerLeftX += MOVE_SPEED;
    // Clamp to bounds (left player can't cross center)
    if (playerLeftX > SCREEN_CENTER - 30)
    { // -30 for player width buffer
        playerLeftX = SCREEN_CENTER - 30;
    }
    if (playerLeftX < LEFT_BOUND)
    {
        playerLeftX = LEFT_BOUND;
    }
}

void Screen2View::playerLeftMoveBackward()
{
    playerLeftX -= MOVE_SPEED;
    // Clamp to bounds
    if (playerLeftX < LEFT_BOUND)
    {
        playerLeftX = LEFT_BOUND;
    }
}

// Player Right Action Functions
void Screen2View::playerRightJump()
{
    if (playerRightOnGround)
    {
        playerRightVelocityY = JUMP_VELOCITY;
        playerRightOnGround = false;
        playerRightJumping = true;
    }
}

void Screen2View::playerRightMoveForward()
{
    playerRightX += MOVE_SPEED;
    // Clamp to bounds
    if (playerRightX > RIGHT_BOUND)
    {
        playerRightX = RIGHT_BOUND;
    }
}

void Screen2View::playerRightMoveBackward()
{
    playerRightX -= MOVE_SPEED;
    // Clamp to bounds (right player can't cross center)
    if (playerRightX < SCREEN_CENTER + 30)
    { // +30 for player width buffer
        playerRightX = SCREEN_CENTER + 30;
    }
    if (playerRightX > RIGHT_BOUND)
    {
        playerRightX = RIGHT_BOUND;
    }
}

void Screen2View::displayCounter(int newCount)
{
    Unicode::snprintf(timeCountBuffer, 3, "%d", newCount);
    timeCount.setWildcard(timeCountBuffer);  // Set the wildcard text
    timeCount.invalidate();
    
    // Debug: Force immediate update to ensure display refresh
    timeCount.resizeToCurrentText();
}

void Screen2View::resetGame()
{
    // Reset player positions to initial positions
    playerLeftX = 55;
    playerLeftY = GROUND_LEVEL;
    playerRightX = 236;
    playerRightY = GROUND_LEVEL;
    
    // Reset physics variables
    playerLeftVelocityY = 0.0f;
    playerRightVelocityY = 0.0f;
    playerLeftOnGround = true;
    playerRightOnGround = true;
    playerLeftJumping = false;
    playerRightJumping = false;
    
    // Reset joystick states
    prevJ1UpPressed = false;
    prevJ2UpPressed = false;
    
    // Update player visual positions
    playerLeft.setXY(playerLeftX, playerLeftY);
    playerRight.setXY(playerRightX, playerRightY);
    
    // Invalidate the entire screen to refresh
    invalidate();
}

void Screen2View::updateScoreDisplay()
{
    if (presenter)
    {
        // Get scores from model through presenter  
        Model* model = static_cast<Screen2Presenter*>(presenter)->getModel();
        if (model)
        {
            int leftScore = model->getLeftPlayerScore();
            int rightScore = model->getRightPlayerScore();
            
            // Update left player score display
            Unicode::snprintf(scoreLeftBuffer, 3, "%d", leftScore);
            pointLeft.setWildcard(scoreLeftBuffer);
            pointLeft.invalidate();
            
            // Update right player score display
            Unicode::snprintf(scoreRightBuffer, 3, "%d", rightScore);
            pointRight.setWildcard(scoreRightBuffer);
            pointRight.invalidate();
        }
    }
}