#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::startGameCounting()
{
    // This method can be called to start the game counting
    // Currently, counting is started when Screen2 becomes active
    // so this method might be used for additional game setup if needed
}