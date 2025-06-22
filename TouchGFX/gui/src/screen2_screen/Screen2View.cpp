#include <gui/screen2_screen/Screen2View.hpp>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    timeCount.setWildcard(timeCountBuffer);
}

void Screen2View::displayCounter(int newCount)
{
    // Định dạng số vào buffer
    Unicode::snprintf(timeCountBuffer, 3, "%d", newCount);

    // Báo cho TouchGFX biết vùng này cần được vẽ lại
    timeCount.invalidate();
}
void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}
