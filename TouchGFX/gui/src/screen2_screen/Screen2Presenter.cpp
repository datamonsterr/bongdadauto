#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

Screen2Presenter::Screen2Presenter(Screen2View& v)
    : view(v)
{

}

void Screen2Presenter::activate()
{
    // Bind this presenter as a listener to the model to receive counter updates
    if (model)
    {
        model->bind(this);
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
