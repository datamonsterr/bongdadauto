#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/Application.hpp>

Screen2Presenter::Screen2Presenter(Screen2View& v)
    : view(v)
{

}

void Screen2Presenter::activate()
{
    // Start the countdown when Screen2 becomes active
    if (model)
    {
        model->startCounting();
    }
}

void Screen2Presenter::deactivate()
{

}

void Screen2Presenter::updateCounter(int newCount)
{
    // Forward the counter update to the view
    view.displayCounter(newCount);
}

void Screen2Presenter::gameEnded()
{
    // Reset the game in the view
    view.resetGame();
    
    // Stop counting
    if (model)
    {
        model->stopCounting();
    }
    
    // Go back to Screen1 with transition
    static_cast<FrontendApplication*>(touchgfx::Application::getInstance())->gotoScreen1ScreenSlideTransitionEast();
}