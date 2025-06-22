#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{
    // Bind this presenter as a listener to the model
    if (model)
    {
        model->bind(this);
    }
}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::startGameCounting()
{
    // Start counting in the model
    if (model)
    {
        model->startCounting();
    }
}
