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
protected:
    //GAME STATE
    bool isGameOver;
    bool isGameStarted;

    //BIRD VARIABLE
    float birdY;
    float birdVelocity;
    static constexpr float gravity = 0.2f;
    static constexpr float jumpForce = -4.0f;

    //PIPE VARIABLE
    float currentPipeSpeed = 2.0f;
    static constexpr int PIPE_DISTANCE = 205;
    static constexpr int SCREEN_WIDTH = 320;

    //SPAWN PIPE
    void randomizePipeHeight(touchgfx::Container& pipeContainer);

    //COLLISION FUNCTION
    bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
    void handleCollision(touchgfx::Container& pipeContainer);
    void setGameOver();

    //SCORE
    int score;
    static constexpr uint16_t SCORE_SIZE = 4;
    Unicode::UnicodeChar scoreBuffer[SCORE_SIZE] = {0};
    Unicode::UnicodeChar highScoreBuffer[SCORE_SIZE] = { 0 };
    Unicode::UnicodeChar endScoreBuffer[SCORE_SIZE] = { 0 };

    //BACKGROUND
    uint8_t dayAlpha;
    uint8_t nightAlpha;
    bool isTransitioning;

    bool isNightMode = false;
    void toggleBackground();
};

#endif // GAMESCREENVIEW_HPP
