#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    
    // Override from Screen1ViewBase - called when startGameButton is clicked
    virtual void startNewGame() override;
    
    // Method to handle joystick data from queue
    void handleJoystickData();
    
    // Method to update welcome text with last game score
    void updateWelcomeText();
    
protected:
    touchgfx::Unicode::UnicodeChar welcomeBuffer1[10];
    touchgfx::Unicode::UnicodeChar welcomeBuffer2[10];
};

#endif // SCREEN1VIEW_HPP