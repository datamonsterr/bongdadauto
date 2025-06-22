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

void Screen1View::startNewGame()
{
    // This method is called when startGameButton is clicked
    // Tell the presenter to start counting
    if (presenter)
    {
        presenter->startGameCounting();
    }
}
