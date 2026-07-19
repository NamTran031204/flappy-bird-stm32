#ifndef GAMESCREEN_CYBERPUNKPRESENTER_HPP
#define GAMESCREEN_CYBERPUNKPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameScreen_CyberPunkView;

class GameScreen_CyberPunkPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GameScreen_CyberPunkPresenter(GameScreen_CyberPunkView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~GameScreen_CyberPunkPresenter() {}

private:
    GameScreen_CyberPunkPresenter();

    GameScreen_CyberPunkView& view;
};

#endif // GAMESCREEN_CYBERPUNKPRESENTER_HPP
