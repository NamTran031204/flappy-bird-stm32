#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();
    void setHighScore(int score) { highScore = score; }
    int getHighScore() { return highScore; }

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
protected:
    ModelListener* modelListener;
    int highScore;
};

#endif // MODEL_HPP
