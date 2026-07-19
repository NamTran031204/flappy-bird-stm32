#ifndef STARTSCREENVIEW_HPP
#define STARTSCREENVIEW_HPP

#include <gui_generated/startscreen_screen/StartScreenViewBase.hpp>
#include <gui/startscreen_screen/StartScreenPresenter.hpp>
#include <gui/common/MenuInput.hpp>

class StartScreenView : public StartScreenViewBase
{
public:
    StartScreenView();
    virtual ~StartScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
protected:
    touchgfx::Unicode::UnicodeChar highScoreBuffer[6];

    // PA0: single-click = vao choi, double-click = mo man chon map.
    ClickDetector clickDetector;
};

#endif // STARTSCREENVIEW_HPP
