#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <images/BitmapDatabase.hpp>

/* Co nut PA0 va HAL_GetTick chi ton tai tren target.
   Simulator khong co FreeRTOS nen chan lai, flap trong simulator dung touch. */
#ifndef SIMULATOR
extern "C"
{
#include "stm32f4xx_hal.h"
extern volatile uint8_t birdPressedFlag;
}
#endif

GameScreenView::GameScreenView()
{

}

void GameScreenView::setupScreen()
{
    GameScreenViewBase::setupScreen();
    resetGame();
}

void GameScreenView::tearDownScreen()
{
    GameScreenViewBase::tearDownScreen();
}

/* Dua toan bo trang thai game ve ban dau, goi moi lan vao GameScreen. */
void GameScreenView::resetGame()
{
    isGameStarted   = false;
    isGameOver      = false;
    isNightMode     = false;
    isTransitioning = false;

    birdVelocity     = 0.0f;
    currentPipeSpeed = 2.0f;
    score            = 0;

    /* Seed RNG dong theo thoi diem vao man, nen moi van khac nhau. */
#ifndef SIMULATOR
    rngState = HAL_GetTick() | 1u;
#else
    rngState = 0x12345u;
#endif

    birdY = (float)Birdo.getY();

    /* Ap skin (nen/ong/chim) theo theme da chon truoc khi bat dau van. */
    applyTheme(presenter->getSelectedTheme());

    GameBackgroundDay.setVisible(true);
    GameBackgroundNight.setVisible(false);
    GameBackgroundDay.invalidate();
    GameBackgroundNight.invalidate();

    GameOverContainer.setVisible(false);
    GameOverContainer.invalidate();

    Unicode::snprintf(ScoreBuffer, SCORE_SIZE, "%d", score);
    Score.invalidate();

    /* 3 ong xep ngoai mep phai, khe ngau nhien, chua ghi diem. */
    Pipe1.setXY(SCREEN_WIDTH, Pipe1.getY());
    randomizePipeHeight(Pipe1);
    scored[0] = false;

    Pipe2.setXY(SCREEN_WIDTH + PIPE_DISTANCE, Pipe2.getY());
    randomizePipeHeight(Pipe2);
    scored[1] = false;

    Pipe3.setXY(SCREEN_WIDTH + 2 * PIPE_DISTANCE, Pipe3.getY());
    randomizePipeHeight(Pipe3);
    scored[2] = false;

    Birdo.moveTo(Birdo.getX(), (int)birdY);
    Birdo.invalidate();
}

/* Vo canh, dung chung cho nut PA0 lan touch. */
void GameScreenView::requestJump()
{
    if (isGameOver)
    {
        return;
    }
    isGameStarted = true;
    birdVelocity = jumpForce;
}

/* Cham man la vo canh cho simulator va board co touch. Nut MainMenu van nhan qua base. */
void GameScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameScreenViewBase::handleClickEvent(evt);

    if (!isGameOver && evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        requestJump();
    }
}

void GameScreenView::handleTickEvent()
{
    GameScreenViewBase::handleTickEvent();

#ifndef SIMULATOR
    /* Nut vat ly PA0 dat co tu ISR EXTI0. Doc khong chan, khong polling GPIO. */
    if (birdPressedFlag)
    {
        birdPressedFlag = 0;
        if (isGameOver)
        {
            application().gotoStartScreenScreenNoTransition();
            return;
        }
        requestJump();
    }
#endif

    const float ground = (float)GROUND - (float)Birdo.getHeight();

    /* GAME OVER, cho chim roi toi dat roi dung. */
    if (isGameOver)
    {
        if (birdY < ground)
        {
            birdVelocity += gravity;
            birdY += birdVelocity;
            if (birdY > ground)
            {
                birdY = ground;
            }
            Birdo.moveTo(Birdo.getX(), (int)birdY);
            Birdo.invalidate();
        }
        return;
    }

    /* WAITING, dung yen cho nhan. */
    if (!isGameStarted)
    {
        return;
    }

    /* Doi nen ngay dem mot lan moi khi toi moc diem. */
    if (isTransitioning)
    {
        GameBackgroundDay.setVisible(!isNightMode);
        GameBackgroundNight.setVisible(isNightMode);
        GameBackgroundDay.invalidate();
        GameBackgroundNight.invalidate();
        isTransitioning = false;
    }

    /* PLAYING, physics chim. */
    birdVelocity += gravity;
    birdY += birdVelocity;
    if (birdY < 0)
    {
        birdY = 0;
        birdVelocity = 0;
    }
    if (birdY >= ground)
    {
        birdY = ground;
        Birdo.moveTo(Birdo.getX(), (int)birdY);
        Birdo.invalidate();
        setGameOver();
        return;
    }
    Birdo.moveTo(Birdo.getX(), (int)birdY);
    Birdo.invalidate();

    /* Cuon va tai su dung 3 ong. */
    touchgfx::Container* pipes[3] = { &Pipe1, &Pipe2, &Pipe3 };
    for (int i = 0; i < 3; i++)
    {
        int x = pipes[i]->getX() - (int)currentPipeSpeed;
        if (x + pipes[i]->getWidth() < 0)
        {
            int rightMost = pipes[0]->getX();
            for (int j = 1; j < 3; j++)
            {
                if (pipes[j]->getX() > rightMost)
                {
                    rightMost = pipes[j]->getX();
                }
            }
            x = rightMost + PIPE_DISTANCE;
            randomizePipeHeight(*pipes[i]);
            scored[i] = false;
        }
        pipes[i]->moveTo(x, pipes[i]->getY());
        pipes[i]->invalidate();
    }

    /* Va cham va ghi diem. */
    handleCollision(Pipe1, 0);
    if (isGameOver) return;
    handleCollision(Pipe2, 1);
    if (isGameOver) return;
    handleCollision(Pipe3, 2);
}

/* AABB chim voi BottomPipe va TopPipe thi game over, voi ScoreZone thi cong 1 diem. */
void GameScreenView::handleCollision(touchgfx::Container& pipe, int idx)
{
    int bX = Birdo.getX();
    int bY = Birdo.getY();
    int bW = Birdo.getWidth();
    int bH = Birdo.getHeight();

    int pX = pipe.getX();
    int pY = pipe.getY();

    touchgfx::Drawable* bottom = pipe.getFirstChild();
    if (bottom == 0)
    {
        return;
    }
    if (checkCollision(bX, bY, bW, bH,
                       pX + bottom->getX(), pY + bottom->getY(),
                       bottom->getWidth(), bottom->getHeight()))
    {
        setGameOver();
        return;
    }

    touchgfx::Drawable* top = bottom->getNextSibling();
    if (top == 0)
    {
        return;
    }
    if (checkCollision(bX, bY, bW, bH,
                       pX + top->getX(), pY + top->getY(),
                       top->getWidth(), top->getHeight()))
    {
        setGameOver();
        return;
    }

    touchgfx::Drawable* zone = top->getNextSibling();
    if (zone == 0)
    {
        return;
    }
    if (!scored[idx] &&
        checkCollision(bX, bY, bW, bH,
                       pX + zone->getX(), pY + zone->getY(),
                       zone->getWidth(), zone->getHeight()))
    {
        scored[idx] = true;
        score++;
        Unicode::snprintf(ScoreBuffer, SCORE_SIZE, "%d", score);
        Score.invalidate();

        if (score % 12 == 0)
        {
            currentPipeSpeed += 0.5f;
            if (currentPipeSpeed > SPEED_MAX)
            {
                currentPipeSpeed = SPEED_MAX;
            }
            toggleBackground();
        }
    }
}

bool GameScreenView::checkCollision(int x1, int y1, int w1, int h1,
                                    int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

void GameScreenView::setGameOver()
{
    isGameOver = true;

    Unicode::snprintf(EndScoreBuffer, ENDSCORE_SIZE, "%d", score);
    EndScore.invalidate();

    int hs = presenter->getHighScore();
    if (score > hs)
    {
        hs = score;
        presenter->saveHighScore(hs);
    }
    Unicode::snprintf(HighScoreBuffer, HIGHSCORE_SIZE, "%d", hs);
    HighScore.invalidate();

    GameOverContainer.setVisible(true);
    GameOverContainer.invalidate();
}

/* Doi Y cua container ong de doi do cao khe.
   Dung LCG tu viet, KHONG dung rand cua newlib vi rand goi abort lam crash tren board. */
void GameScreenView::randomizePipeHeight(touchgfx::Container& pipe)
{
    const int DEFAULT_Y = -35;
    rngState = rngState * 1103515245u + 12345u;
    int offset = (int)((rngState >> 16) % 81u) - 50;
    pipe.setY(DEFAULT_Y + offset);
}

void GameScreenView::toggleBackground()
{
    isNightMode = !isNightMode;
    isTransitioning = true;
}

/* Doi bitmap nen/ong/chim theo theme. 0 = Classic, 1 = Cyberpunk.
   Day/Night widget mang bitmap cua theme hien tai nen toggleBackground lat ngay/dem tu dung. */
void GameScreenView::applyTheme(int theme)
{
    touchgfx::BitmapId bgDay, bgNight, bird, pipeBottom, pipeTop;
    if (theme == 1)   // Cyberpunk
    {
        bgDay      = BITMAP_BACKGROUND_DAY_CYBERPUNK_ID;
        bgNight    = BITMAP_BACKGROUND_NIGHT_CYBERPUNK_ID;
        bird       = BITMAP_BIRD_YELLOW_ID;
        pipeBottom = BITMAP_PIPE_CYBERPUNK_ID;
        pipeTop    = BITMAP_PIPE_BOTTOM_CYBERPUNK_ID;
    }
    else              // Classic (0)
    {
        bgDay      = BITMAP_BACKGROUND_DAY_ID;
        bgNight    = BITMAP_BACKGROUND_NIGHT_ID;
        bird       = BITMAP_BIRD__ID;
        pipeBottom = BITMAP_PIPE_ID;      // Pipe_.png doi ten thanh Pipe.png -> BITMAP_PIPE_ID
        pipeTop    = BITMAP_PIPE_BOTTOM_ID;
    }

    GameBackgroundDay.setBitmap(touchgfx::Bitmap(bgDay));
    GameBackgroundNight.setBitmap(touchgfx::Bitmap(bgNight));
    Birdo.setBitmap(touchgfx::Bitmap(bird));

    BottomPipe.setBitmap(touchgfx::Bitmap(pipeBottom));
    BottomPipe_1.setBitmap(touchgfx::Bitmap(pipeBottom));
    BottomPipe_2.setBitmap(touchgfx::Bitmap(pipeBottom));
    TopPipe.setBitmap(touchgfx::Bitmap(pipeTop));
    TopPipe_1.setBitmap(touchgfx::Bitmap(pipeTop));
    TopPipe_2.setBitmap(touchgfx::Bitmap(pipeTop));

    GameBackgroundDay.invalidate();
    GameBackgroundNight.invalidate();
    Birdo.invalidate();
    BottomPipe.invalidate();
    BottomPipe_1.invalidate();
    BottomPipe_2.invalidate();
    TopPipe.invalidate();
    TopPipe_1.invalidate();
    TopPipe_2.invalidate();
}
