#include <gui/gamescreen_screen/GameScreenView.hpp>

extern "C"
{
    #include "cmsis_os2.h"
    extern osMessageQueueId_t birdQueueHandle;
}

static int savedHighScore = 0;

GameScreenView::GameScreenView()
{

}

void GameScreenView::setupScreen()
{
    GameScreenViewBase::setupScreen();

    currentPipeSpeed = 2;

    isNightMode = false;
    isTransitioning = false;

    score = 0;
    Score.setWildcard(scoreBuffer);
    Unicode::snprintf(scoreBuffer, SCORE_SIZE, "%d", score);
    Score.invalidate();


    HighScore.setWildcard(highScoreBuffer);
    Unicode::snprintf(highScoreBuffer, SCORE_SIZE, "%d", savedHighScore);
    HighScore.invalidate();

    birdY = Birdo.getY();
    birdVelocity = 0;
    isGameOver = false;
    isGameStarted = false;

    GameOverContainer.setVisible(false);
    GameOverContainer.invalidate();

    Pipe1.setXY(SCREEN_WIDTH, Pipe1.getY());
    randomizePipeHeight(Pipe1);

    Pipe2.setXY(SCREEN_WIDTH + PIPE_DISTANCE, Pipe2.getY());
    randomizePipeHeight(Pipe2);

    Pipe3.setXY(SCREEN_WIDTH + (PIPE_DISTANCE * 2), Pipe3.getY());
    randomizePipeHeight(Pipe3);
}

void GameScreenView::tearDownScreen()
{
    GameScreenViewBase::tearDownScreen();
}

void GameScreenView::handleTickEvent()
{
	GameScreenViewBase::handleTickEvent();

	//GAMEOVER
	if(isGameOver){
		if(birdY < 240 - Birdo.getHeight()){
			birdVelocity += gravity;
			birdY += birdVelocity;
			if (birdY > 240 - Birdo.getHeight())
			{
				birdY = 240 - Birdo.getHeight();
			}
			Birdo.moveTo(Birdo.getX(), (int)birdY);
			Birdo.invalidate();
		}
		return;
	}


	//JUMP
	uint8_t jumpCmd = 0;

	if (birdQueueHandle != NULL && osMessageQueueGet(birdQueueHandle, &jumpCmd, NULL, 0) == osOK)
	{
	    if (jumpCmd == 1)
	    {
	    	if(!isGameStarted){
	    		isGameStarted = true;
	    	}
	        birdVelocity = jumpForce;
	    }
	}

	if(!isGameStarted){
		return;
	}

	if (isTransitioning) {
	    GameBackgroundDay.setVisible(!isNightMode);
	    GameBackgroundNight.setVisible(isNightMode);

	    GameBackgroundDay.invalidate();
	    GameBackgroundNight.invalidate();

	    isTransitioning = false;
	}

	if(isGameStarted && !isGameOver){
		//BIRD LOCATION
		birdVelocity += gravity;
		birdY += birdVelocity;

		if (birdY < 0)
		{
			birdY = 0;
			 birdVelocity = 0;
		}

		float groundLevel = 240 - (float)Birdo.getHeight();

		if (birdY >= groundLevel)
		{
		    birdY = groundLevel;
		    setGameOver();
		}
		Birdo.moveTo(Birdo.getX(), (int)birdY);
		Birdo.invalidate();

		//PIPE SPAWN
		int pipe1X = Pipe1.getX() - currentPipeSpeed;
		if (pipe1X + Pipe1.getWidth() < 0)
		{
		    pipe1X = Pipe3.getX() + PIPE_DISTANCE;
		     randomizePipeHeight(Pipe1);
		}
		Pipe1.moveTo(pipe1X, Pipe1.getY());
		Pipe1.invalidate();

		int pipe2X = Pipe2.getX() - currentPipeSpeed;
		if (pipe2X + Pipe2.getWidth() < 0)
		{
		     pipe2X = Pipe1.getX() + PIPE_DISTANCE;
		     randomizePipeHeight(Pipe2);
		}
		Pipe2.moveTo(pipe2X, Pipe2.getY());
		Pipe2.invalidate();

		int pipe3X = Pipe3.getX() - currentPipeSpeed;
		if (pipe3X + Pipe3.getWidth() < 0)
		{
		    pipe3X = Pipe2.getX() + PIPE_DISTANCE;
		    randomizePipeHeight(Pipe3);
		}
		Pipe3.moveTo(pipe3X, Pipe3.getY());
		Pipe3.invalidate();

		//COLLISION CHECK
		handleCollision(Pipe1);
		handleCollision(Pipe2);
		handleCollision(Pipe3);
	}


}

void GameScreenView::handleCollision(touchgfx::Container& pipeContainer)
{
    int bX = Birdo.getX();
    int bY = Birdo.getY();
    int bW = Birdo.getWidth();
    int bH = Birdo.getHeight();

    int pX = pipeContainer.getX();
    int pY = pipeContainer.getY();

    touchgfx::Drawable* botPipePtr = pipeContainer.getFirstChild();
    if (botPipePtr != nullptr)
    {
        int botPipeY = pY + botPipePtr->getY();
        int botPipeX = pX + botPipePtr->getX();
        int botPipeW = botPipePtr->getWidth();
        int botPipeH = botPipePtr->getHeight();

        if (checkCollision(bX, bY, bW, bH, botPipeX, botPipeY, botPipeW, botPipeH))
        {
            setGameOver();
            return;
        }

        touchgfx::Drawable* topPipePtr = botPipePtr->getNextSibling();
        if (topPipePtr != nullptr)
        {
            int topPipeY = pY + topPipePtr->getY();
            int topPipeX = pX + topPipePtr->getX();
            int topPipeW = topPipePtr->getWidth();
            int topPipeH = topPipePtr->getHeight();

            if (checkCollision(bX, bY, bW, bH, topPipeX, topPipeY, topPipeW, topPipeH))
            {
                setGameOver();
                return;
            }

            touchgfx::Drawable* scoreZonePtr = topPipePtr->getNextSibling();
            if (scoreZonePtr != nullptr)
            {
                int scoreZoneY = pY + scoreZonePtr->getY();
                int scoreZoneX = pX + scoreZonePtr->getX();
                int scoreZoneW = scoreZonePtr->getWidth();
                int scoreZoneH = scoreZonePtr->getHeight();

                if (checkCollision(bX, bY, bW, bH, scoreZoneX, scoreZoneY, scoreZoneW, scoreZoneH))
                {
                    static int lastScoredPipeId = -1;
                    int currentPipeId = (int)&pipeContainer;

                    if (lastScoredPipeId != currentPipeId)
                    {
                        score++;
                        if (score % 12 == 0)
                        {
                           currentPipeSpeed+=0.5;
                           toggleBackground();
                        }
                        if (currentPipeSpeed > 5) currentPipeSpeed = 5;
                        lastScoredPipeId = currentPipeId;


                        Unicode::snprintf(scoreBuffer, SCORE_SIZE, "%d", score);
                        Score.invalidate();
                    }
                }
            }
        }
    }
}

bool GameScreenView::checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

void GameScreenView::setGameOver()
{
    isGameOver = true;

    EndScore.setTypedText(touchgfx::TypedText(Score.getTypedText()));
    EndScore.setWildcard(endScoreBuffer);
    Unicode::snprintf(endScoreBuffer, SCORE_SIZE, "%d", score);
    EndScore.invalidate();

    if(score > savedHighScore){
    	savedHighScore = score;
    	presenter->saveHighScore(savedHighScore);
    }

    HighScore.setWildcard(highScoreBuffer);
    Unicode::snprintf(highScoreBuffer, SCORE_SIZE, "%d", savedHighScore);
    HighScore.invalidate();

    GameOverContainer.setVisible(true);
    GameOverContainer.invalidate();
}

void GameScreenView::randomizePipeHeight(touchgfx::Container& pipeContainer)
{
    const int DEFAULT_Y = -35;

    static unsigned int seed = 123456789;
    seed = seed * 1103515245 + 12345;
    int randomValue = (seed / 65536) % 81;

    int offset = randomValue - 50;
    int finalY = DEFAULT_Y + offset;

    pipeContainer.setY(finalY);
}

void GameScreenView::toggleBackground()
{
	isNightMode = !isNightMode;
	isTransitioning = true;
}
