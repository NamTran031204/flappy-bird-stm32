#ifndef SELECTMAPVIEW_HPP
#define SELECTMAPVIEW_HPP

#include <gui_generated/selectmap_screen/SelectMapViewBase.hpp>
#include <gui/selectmap_screen/SelectMapPresenter.hpp>

class SelectMapView : public SelectMapViewBase
{
public:
    SelectMapView();
    virtual ~SelectMapView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SELECTMAPVIEW_HPP
