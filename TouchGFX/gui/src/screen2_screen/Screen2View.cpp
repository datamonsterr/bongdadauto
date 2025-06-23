#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/model/Model.hpp>
#include "cmsis_os.h"
#include "joystick_data.h"
#include <cmath>
#include <cstdlib>  // For abs function

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
    
    // Initialize ball position
    ballX = BALL_INITIAL_X;
    ballY = BALL_INITIAL_Y;
    ballVelocityX = 0.0f;
    ballVelocityY = 0.0f;
    ball.setXY(ballX, ballY);
    
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
    
    // Initialize goal detection cooldown
    goalCooldown = 0;
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
    
    // Update ball physics
    updateBallPhysics();
    
    // Check for player-ball collisions
    checkPlayerBallCollision();
    
    // Check for goal detection
    checkGoalDetection();
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
    
    // Reset ball position
    ballX = BALL_INITIAL_X;
    ballY = BALL_INITIAL_Y;
    ballVelocityX = 0.0f;
    ballVelocityY = 0.0f;
    ball.setXY(ballX, ballY);
    
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
    
    // Reset goal detection cooldown
    goalCooldown = 0;
    
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

void Screen2View::checkGoalDetection()
{
    // Decrease goal cooldown if active
    if (goalCooldown > 0)
    {
        goalCooldown--;
        return; // Skip goal detection during cooldown
    }
    
    // Get current ball position from the ball widget
    ballX = ball.getX();
    ballY = ball.getY();
    
    // Check if ball is in left goal area (right player scores)
    // Only count as goal if ball is within the goal Y boundaries
    if (ballX >= LEFT_GOAL_MIN_X && ballX <= LEFT_GOAL_MAX_X && 
        ballY >= GOAL_MIN_Y && ballY <= GOAL_MAX_Y)
    {
        // Right player scored!
        if (presenter)
        {
            Model* model = static_cast<Screen2Presenter*>(presenter)->getModel();
            if (model)
            {
                model->incrementRightPlayerScore();
                updateScoreDisplay();
            }
        }
        // Reset ball position after goal
        resetBallPosition();
        // Set cooldown to prevent multiple rapid detections
        goalCooldown = GOAL_COOLDOWN_FRAMES;
    }
    // Check if ball is in right goal area (left player scores)
    // Only count as goal if ball is within the goal Y boundaries
    else if (ballX >= RIGHT_GOAL_MIN_X && ballX <= RIGHT_GOAL_MAX_X && 
             ballY >= GOAL_MIN_Y && ballY <= GOAL_MAX_Y)
    {
        // Left player scored!
        if (presenter)
        {
            Model* model = static_cast<Screen2Presenter*>(presenter)->getModel();
            if (model)
            {
                model->incrementLeftPlayerScore();
                updateScoreDisplay();
            }
        }
        // Reset ball position after goal
        resetBallPosition();
        // Set cooldown to prevent multiple rapid detections
        goalCooldown = GOAL_COOLDOWN_FRAMES;
    }
}

void Screen2View::resetBallPosition()
{
    // Reset ball to center position
    ballX = BALL_INITIAL_X;
    ballY = BALL_INITIAL_Y;
    ballVelocityX = 0.0f;
    ballVelocityY = 0.0f;
    ball.setXY(ballX, ballY);
    
    // Invalidate ball area to refresh display
    touchgfx::Rect ballRect(ballX - 5, ballY - 5, BALL_SIZE + 10, BALL_SIZE + 10);
    invalidateRect(ballRect);
}

void Screen2View::updateBallPhysics()
{
    // Apply gravity to ball (always pulling downward)
    ballVelocityY += BALL_GRAVITY;
    
    // Apply velocity to ball position
    ballX += (int)ballVelocityX;
    ballY += (int)ballVelocityY;
    
    // Check screen boundaries and bounce
    // Left edge
    if (ballX <= 0)
    {
        ballX = 0;
        ballVelocityX = -ballVelocityX * BALL_BOUNCE_DAMPING;
    }
    // Right edge
    else if (ballX >= (RIGHT_BOUND - BALL_SIZE))
    {
        ballX = RIGHT_BOUND - BALL_SIZE;
        ballVelocityX = -ballVelocityX * BALL_BOUNCE_DAMPING;
    }
    
    // Top edge
    if (ballY <= 0)
    {
        ballY = 0;
        ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
    }
    // Bottom edge (ground level)
    else if (ballY >= (GROUND_LEVEL + 39))  // 39 is ground offset for ball
    {
        ballY = GROUND_LEVEL + 39;
        ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
        // Ensure ball doesn't get stuck bouncing on ground
        if (abs(ballVelocityY) < 2.0f)
        {
            ballVelocityY = 0.0f;
        }
    }
    
    // Check goal boundaries and bounce (treat like screen edges)
    // Left goal area - bounce off goal boundaries
    if (ballX >= LEFT_GOAL_MIN_X && ballX <= LEFT_GOAL_MAX_X)
    {
        if (ballY < GOAL_MIN_Y)
        {
            ballY = GOAL_MIN_Y;
            ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
        }
        else if (ballY > GOAL_MAX_Y)
        {
            ballY = GOAL_MAX_Y;
            ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
        }
    }
    
    // Right goal area - bounce off goal boundaries  
    if (ballX >= RIGHT_GOAL_MIN_X && ballX <= RIGHT_GOAL_MAX_X)
    {
        if (ballY < GOAL_MIN_Y)
        {
            ballY = GOAL_MIN_Y;
            ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
        }
        else if (ballY > GOAL_MAX_Y)
        {
            ballY = GOAL_MAX_Y;
            ballVelocityY = -ballVelocityY * BALL_BOUNCE_DAMPING;
        }
    }
    
    // Apply friction to horizontal movement only (not vertical due to gravity)
    ballVelocityX *= BALL_FRICTION;
    
    // Stop very slow horizontal movements, but always allow vertical movement due to gravity
    if (abs(ballVelocityX) < BALL_MIN_VELOCITY)
        ballVelocityX = 0.0f;
    // Don't stop vertical velocity - let gravity always work
    
    // Update ball widget position
    ball.setXY(ballX, ballY);
    
    // Invalidate ball area for redraw
    touchgfx::Rect ballRect(ballX - 5, ballY - 5, BALL_SIZE + 10, BALL_SIZE + 10);
    invalidateRect(ballRect);
}

void Screen2View::checkPlayerBallCollision()
{
    // Check collision with left player
    if (isColliding(playerLeftX, playerLeftY, PLAYER_LEFT_WIDTH, PLAYER_LEFT_HEIGHT,
                    ballX, ballY, BALL_SIZE, BALL_SIZE))
    {
        // Calculate shoot direction based on player position relative to ball
        float shootX = (ballX - (playerLeftX + PLAYER_LEFT_WIDTH/2)) * 0.3f;
        float shootY = (ballY - (playerLeftY + PLAYER_LEFT_HEIGHT/2)) * 0.3f;
        
        // Add some upward force for more realistic ball movement
        shootY -= 2.0f;
        
        // Apply the shoot
        shoot(shootX, shootY);
    }
    
    // Check collision with right player
    if (isColliding(playerRightX, playerRightY, PLAYER_RIGHT_WIDTH, PLAYER_RIGHT_HEIGHT,
                    ballX, ballY, BALL_SIZE, BALL_SIZE))
    {
        // Calculate shoot direction based on player position relative to ball
        float shootX = (ballX - (playerRightX + PLAYER_RIGHT_WIDTH/2)) * 0.3f;
        float shootY = (ballY - (playerRightY + PLAYER_RIGHT_HEIGHT/2)) * 0.3f;
        
        // Add some upward force for more realistic ball movement
        shootY -= 2.0f;
        
        // Apply the shoot
        shoot(shootX, shootY);
    }
}

bool Screen2View::isColliding(int obj1X, int obj1Y, int obj1Width, int obj1Height,
                              int obj2X, int obj2Y, int obj2Width, int obj2Height)
{
    return (obj1X < obj2X + obj2Width &&
            obj1X + obj1Width > obj2X &&
            obj1Y < obj2Y + obj2Height &&
            obj1Y + obj1Height > obj2Y);
}

void Screen2View::shoot(float velocityX, float velocityY)
{
    // Apply the velocity to the ball
    ballVelocityX = velocityX;
    ballVelocityY = velocityY;
    
    // Clamp maximum velocity to prevent ball from moving too fast
    const float MAX_VELOCITY = 12.0f;
    if (ballVelocityX > MAX_VELOCITY) ballVelocityX = MAX_VELOCITY;
    if (ballVelocityX < -MAX_VELOCITY) ballVelocityX = -MAX_VELOCITY;
    if (ballVelocityY > MAX_VELOCITY) ballVelocityY = MAX_VELOCITY;
    if (ballVelocityY < -MAX_VELOCITY) ballVelocityY = -MAX_VELOCITY;
}