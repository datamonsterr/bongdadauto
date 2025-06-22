#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
<<<<<<< HEAD

void Screen1View::startNewGame()
{
    // This method is called when startGameButton is clicked
    // Tell the presenter to start counting
    if (presenter)
    {
        presenter->startGameCounting();
    }
}

void Screen1View::startNewGame()
{
    // This method is called when startGameButton is clicked
    // Tell the presenter to start counting
    if (presenter)
    {
        presenter->startGameCounting();
    }
}

void Screen1View::handleJoystickData()
{
    JoystickData_t joystick_data;
    
    // Try to get joystick data from queue (non-blocking)
    osStatus_t status = osMessageQueueGet(joystickDataQueue, &joystick_data, NULL, 0);
    
    if (status == osOK) {
        // Successfully received joystick data
        // Handle the joystick data here
        // joystick_data.j1_x, joystick_data.j1_y, joystick_data.j2_x, joystick_data.j2_y
        
        // TODO: Add your joystick handling logic here
        // For example: update UI elements based on joystick values
    }
}
=======
>>>>>>> origin/main
