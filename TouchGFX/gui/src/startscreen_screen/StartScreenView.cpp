#include <gui/startscreen_screen/StartScreenView.hpp>

StartScreenView::StartScreenView()
{

}

void StartScreenView::setupScreen()
{
    StartScreenViewBase::setupScreen();
    int topScore = presenter->getHighScore();

    HighScore.setWildcard(mainMenuHighScoreBuffer);
    Unicode::snprintf(mainMenuHighScoreBuffer, 5, "%d", topScore);
    HighScore.invalidate();

}

void StartScreenView::tearDownScreen()
{
    StartScreenViewBase::tearDownScreen();
}
