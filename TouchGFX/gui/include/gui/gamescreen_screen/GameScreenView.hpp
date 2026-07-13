#ifndef GAMESCREENVIEW_HPP
#define GAMESCREENVIEW_HPP

#include <gui_generated/gamescreen_screen/GameScreenViewBase.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>

class GameScreenView : public GameScreenViewBase
{
public:
    GameScreenView();
    virtual ~GameScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);

protected:
    // Trang thai game
    bool isGameStarted;
    bool isGameOver;
    bool isNightMode;
    bool isTransitioning;

    // Chim
    float birdY;
    float birdVelocity;
    static constexpr float gravity   = 0.2f;
    static constexpr float jumpForce = -4.0f;

    // Ong
    float currentPipeSpeed;
    static constexpr int   PIPE_DISTANCE = 205;
    static constexpr int   SCREEN_WIDTH  = 320;
    static constexpr int   GROUND        = 240;   // day man o che do landscape
    static constexpr float SPEED_MAX     = 5.0f;

    // Diem
    int  score;
    bool scored[3];   // moi pipe da ghi diem chua, reset khi tai su dung

    // RNG tu viet kieu LCG, khong dung rand cua newlib vi rand crash tren board
    uint32_t rngState;

    // Ham logic
    void resetGame();
    void requestJump();
    void randomizePipeHeight(touchgfx::Container& pipe);
    void handleCollision(touchgfx::Container& pipe, int idx);
    bool checkCollision(int x1, int y1, int w1, int h1,
                        int x2, int y2, int w2, int h2);
    void setGameOver();
    void toggleBackground();
};

#endif // GAMESCREENVIEW_HPP
