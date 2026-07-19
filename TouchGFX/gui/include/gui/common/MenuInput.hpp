#ifndef GUI_COMMON_MENUINPUT_HPP
#define GUI_COMMON_MENUINPUT_HPP

#include <cstdint>

/*
 * Helper thuan logic cho dieu khien menu bang mot nut vat ly (PA0):
 * - double-click (2 cu cach nhau <= DOUBLE_WINDOW ms) => NAVIGATE (doi focus)
 * - single-click (1 cu, khong co cu 2 trong DOUBLE_WINDOW ms) => SELECT (kich hoat muc dang focus)
 *
 * Khong include HAL, khong phu thuoc TouchGFX -> build duoc ca tren simulator.
 * Xem SPEC-levelselect-theme.md Sec 5.3.
 */

enum class MenuAction
{
    NONE,
    NAVIGATE,
    SELECT
};

class ClickDetector
{
    uint32_t t1 = 0;
    bool waiting = false;
    static constexpr uint32_t DOUBLE_WINDOW = 300; // ms

public:
    MenuAction update(bool pressedThisTick, uint32_t now)
    {
        if (pressedThisTick)
        {
            if (waiting && (now - t1) <= DOUBLE_WINDOW)
            {
                waiting = false;
                return MenuAction::NAVIGATE; // cu thu 2 -> double -> dieu huong
            }
            t1 = now;
            waiting = true;
            return MenuAction::NONE; // cu dau -> cho xem co cu 2
        }
        if (waiting && (now - t1) > DOUBLE_WINDOW)
        {
            waiting = false;
            return MenuAction::SELECT; // het cua so, khong cu 2 -> single -> chon
        }
        return MenuAction::NONE;
    }
};

#endif // GUI_COMMON_MENUINPUT_HPP
