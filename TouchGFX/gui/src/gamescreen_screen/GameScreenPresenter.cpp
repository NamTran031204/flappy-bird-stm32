#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>

GameScreenPresenter::GameScreenPresenter(GameScreenView& v)
    : view(v)
{

}

void GameScreenPresenter::activate()
{

}

void GameScreenPresenter::deactivate()
{

}

void GameScreenPresenter::saveHighScore(int score)
{
    model->setHighScore(score);
}

int GameScreenPresenter::getHighScore()
{
    return model->getHighScore();
}

int GameScreenPresenter::getSelectedTheme()
{
    return model->getSelectedTheme();
}
