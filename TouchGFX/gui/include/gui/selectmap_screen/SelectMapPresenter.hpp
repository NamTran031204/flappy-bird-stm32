#ifndef SELECTMAPPRESENTER_HPP
#define SELECTMAPPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectMapView;

class SelectMapPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SelectMapPresenter(SelectMapView& v);

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

    virtual ~SelectMapPresenter() {}

    int getSelectedTheme();
    void setSelectedTheme(int theme);

private:
    SelectMapPresenter();

    SelectMapView& view;
};

#endif // SELECTMAPPRESENTER_HPP
