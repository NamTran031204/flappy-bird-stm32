#ifndef GAMESCREEN_CYBERPUNKVIEW_HPP
#define GAMESCREEN_CYBERPUNKVIEW_HPP

#include <gui_generated/gamescreen_cyberpunk_screen/GameScreen_CyberPunkViewBase.hpp>
#include <gui/gamescreen_cyberpunk_screen/GameScreen_CyberPunkPresenter.hpp>

class GameScreen_CyberPunkView : public GameScreen_CyberPunkViewBase
{
public:
    GameScreen_CyberPunkView();
    virtual ~GameScreen_CyberPunkView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // GAMESCREEN_CYBERPUNKVIEW_HPP
