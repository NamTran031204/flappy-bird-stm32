#include <gui/startscreen_screen/StartScreenView.hpp>

/* Co nut PA0 dinh nghia trong Core Src main.c, chi ton tai tren target.
   Simulator khong co FreeRTOS nen chan lai, dieu huong bang touch. */
#ifndef SIMULATOR
extern "C"
{
extern volatile uint8_t birdPressedFlag;
}
#endif

StartScreenView::StartScreenView()
{

}

void StartScreenView::setupScreen()
{
    StartScreenViewBase::setupScreen();

    /* Hien thi high score cao nhat, doc tu Model qua Presenter. */
    HighScore.setWildcard(highScoreBuffer);
    Unicode::snprintf(highScoreBuffer, 6, "%d", presenter->getHighScore());
    HighScore.invalidate();
}

void StartScreenView::tearDownScreen()
{
    StartScreenViewBase::tearDownScreen();
}

void StartScreenView::handleTickEvent()
{
    StartScreenViewBase::handleTickEvent();

#ifndef SIMULATOR
    /* Nut vat ly PA0 dat co. Hanh xu nhu bam startBtn, chuyen sang GameScreen. */
    if (birdPressedFlag)
    {
        birdPressedFlag = 0;
        application().gotoGameScreenScreenNoTransition();
    }
#endif
}
