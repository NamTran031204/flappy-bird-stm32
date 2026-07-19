#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void setHighScore(int score) { highScore = score; }
    int getHighScore() const { return highScore; }

    void setSelectedTheme(int t) { selectedTheme = t; }
    int getSelectedTheme() const { return selectedTheme; }

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
protected:
    ModelListener* modelListener;
    int highScore;
    int selectedTheme;
};

#endif // MODEL_HPP
