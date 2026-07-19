#include <gui/startscreen_screen/StartScreenView.hpp>

/* Co nut PA0 va HAL_GetTick chi ton tai tren target.
   Simulator khong co FreeRTOS nen chan lai, dieu huong bang touch. */
#ifndef SIMULATOR
extern "C"
{
#include "stm32f4xx_hal.h"
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
    /* Doc co PA0 tu ISR EXTI0 (khong polling GPIO), phan biet single/double bang HAL_GetTick (ngat TIM6).
       single-click = vao choi ngay; double-click = mo man chon map. */
    bool pressed = birdPressedFlag;
    birdPressedFlag = 0;

    switch (clickDetector.update(pressed, HAL_GetTick()))
    {
    case MenuAction::NAVIGATE:
        application().gotoSelectMapScreenNoTransition();
        break;
    case MenuAction::SELECT:
        application().gotoGameScreenScreenNoTransition();
        break;
    default:
        break;
    }
#endif
}
