#include <gui/selectmap_screen/SelectMapView.hpp>
#include <touchgfx/events/ClickEvent.hpp>

/* Nut PA0 va HAL_GetTick chi ton tai tren target.
   Simulator khong co FreeRTOS nen chan lai, dieu huong bang touch. */
#ifndef SIMULATOR
extern "C"
{
#include "stm32f4xx_hal.h"
extern volatile uint8_t birdPressedFlag;
}
#endif

SelectMapView::SelectMapView()
    : focusIndex(0)
{

}

void SelectMapView::setupScreen()
{
    SelectMapViewBase::setupScreen();

    themes[0] = &theme0;
    themes[1] = &theme1;

    /* Focus mac dinh = theme dang chon hien tai (Model giu qua cac lan chuyen man). */
    focusIndex = presenter->getSelectedTheme();
    if (focusIndex < 0 || focusIndex >= THEME_COUNT)
    {
        focusIndex = 0;
    }
    updateFocusArrow();
}

void SelectMapView::tearDownScreen()
{
    SelectMapViewBase::tearDownScreen();
}

/* Dat con tro focusArrow can giua theo chieu ngang duoi card dang focus. */
void SelectMapView::updateFocusArrow()
{
    touchgfx::Drawable* t = themes[focusIndex];
    int cx = t->getX() + t->getWidth() / 2;
    focusArrow.moveTo(cx - focusArrow.getWidth() / 2, focusArrow.getY());
    focusArrow.invalidate();
}

void SelectMapView::handleTickEvent()
{
    SelectMapViewBase::handleTickEvent();

#ifndef SIMULATOR
    /* Doc co PA0 tu ISR EXTI0 (khong polling GPIO), phan biet single/double bang HAL_GetTick (ngat TIM6). */
    bool pressed = birdPressedFlag;
    birdPressedFlag = 0;

    switch (clickDetector.update(pressed, HAL_GetTick()))
    {
    case MenuAction::NAVIGATE:
        /* Double-click: chuyen focus sang theme ke, vong tron. */
        focusIndex = (focusIndex + 1) % THEME_COUNT;
        updateFocusArrow();
        break;
    case MenuAction::SELECT:
        /* Single-click: chon theme dang focus, luu vao Model roi ve Start. */
        presenter->setSelectedTheme(focusIndex);
        application().gotoStartScreenScreenNoTransition();
        break;
    default:
        break;
    }
#endif
}

/* Simulator dung touch: cham vao card theme nao thi chon theme do roi ve Start.
   Tren target LCD-only khong co touch nen handler nay khong chay. */
void SelectMapView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    SelectMapViewBase::handleClickEvent(evt);

    if (evt.getType() != touchgfx::ClickEvent::PRESSED)
    {
        return;
    }

    int x = evt.getX();
    int y = evt.getY();
    for (int i = 0; i < THEME_COUNT; i++)
    {
        touchgfx::Drawable* t = themes[i];
        if (x >= t->getX() && x < t->getX() + t->getWidth() &&
            y >= t->getY() && y < t->getY() + t->getHeight())
        {
            presenter->setSelectedTheme(i);
            application().gotoStartScreenScreenNoTransition();
            return;
        }
    }
}
