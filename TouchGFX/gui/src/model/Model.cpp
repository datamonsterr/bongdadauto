#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0), tickCounter(0), counter90to0(90), countingActive(false)
{

}

void Model::tick()
{
	// Only count if counting is active
	if (!countingActive)
		return;
		
	tickCounter++;
	// STM32F429 runs at ~24Hz, 24 ticks is 1 second
	if (tickCounter % 60 == 0)
	{
		// Tăng biến đếm và quay vòng
		counter90to0--;
		if (counter90to0 < 0)
		{
			counter90to0 = 90;
		}

		// Thông báo cho Presenter (nếu có) rằng giá trị đã thay đổi
		if (modelListener)
		{
			modelListener->updateCounter(counter90to0);
		}
	}
}

void Model::startCounting()
{
	countingActive = true;
	tickCounter = 0;  // Reset tick counter when starting
	counter90to0 = 90;  // Reset counter to 90
	
	// Immediately update the display with initial value
	if (modelListener)
	{
		modelListener->updateCounter(counter90to0);
	}
}

void Model::stopCounting()
{
	countingActive = false;
}