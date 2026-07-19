#ifndef SELECTMAPVIEW_HPP
#define SELECTMAPVIEW_HPP

#include <gui_generated/selectmap_screen/SelectMapViewBase.hpp>
#include <gui/selectmap_screen/SelectMapPresenter.hpp>
#include <gui/common/MenuInput.hpp>

class SelectMapView : public SelectMapViewBase
{
public:
    SelectMapView();
    virtual ~SelectMapView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);
protected:
    // So theme dang co (theme0, theme1). Viet tong quat de de mo rong.
    static const int THEME_COUNT = 2;

    int focusIndex;                          // theme dang duoc tro toi (single-click se chon)
    ClickDetector clickDetector;             // may trang thai single/double click PA0
    touchgfx::Drawable* themes[THEME_COUNT]; // tro toi cac card theme cua base

    // Dat con tro focusArrow vao card themes[focusIndex].
    void updateFocusArrow();
};

#endif // SELECTMAPVIEW_HPP
