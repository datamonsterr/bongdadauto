#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/model/Model.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    
    // Update welcome text with last game score
    updateWelcomeText();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::startNewGame()
{
    // Reset scores when starting new game
    if (presenter && presenter->getModel())
    {
        presenter->getModel()->resetScores();
        // Note: We don't reset gameCompleted flag here so previous score remains visible
    }
    
    // This method is called when startGameButton is clicked
    // Tell the presenter to start counting
    if (presenter)
    {
        presenter->startGameCounting();
    }
}

void Screen1View::updateWelcomeText()
{
    if (presenter && presenter->getModel())
    {
        Model* model = presenter->getModel();
        
        // Check if any game has been completed
        if (model->hasGameBeenCompleted())
        {
            // Show last game scores
            int leftScore = model->getLeftPlayerScore();
            int rightScore = model->getRightPlayerScore();
            
            // Format score as "X-Y"
            Unicode::snprintf(welcomeBuffer1, 10, "%d", leftScore);
            Unicode::snprintf(welcomeBuffer2, 10, "  %d", rightScore);
        }
        else
        {
            // Show "WELCOME" for first time or when no game completed yet
            Unicode::snprintf(welcomeBuffer1, 10, "WELCOME");
            Unicode::snprintf(welcomeBuffer2, 10, "");
        }
        
        // Update the text area with wildcards
        textArea1.setWildcard1(welcomeBuffer1);
        textArea1.setWildcard2(welcomeBuffer2);
        textArea1.invalidate();
    }
    else
    {
        // Fallback to "WELCOME" if no model available
        Unicode::snprintf(welcomeBuffer1, 10, "WELCOME");
        Unicode::snprintf(welcomeBuffer2, 10, "");
        
        textArea1.setWildcard1(welcomeBuffer1);
        textArea1.setWildcard2(welcomeBuffer2);
        textArea1.invalidate();
    }
}
