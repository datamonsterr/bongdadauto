#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    
    // Methods to control counting
    void startCounting();
    void stopCounting();
    bool isCountingActive() const { return countingActive; }
    
    // Game score management
    int getLeftPlayerScore() const { return leftPlayerScore; }
    int getRightPlayerScore() const { return rightPlayerScore; }
    void incrementLeftPlayerScore() { leftPlayerScore++; }
    void incrementRightPlayerScore() { rightPlayerScore++; }
    void resetScores() { leftPlayerScore = 0; rightPlayerScore = 0; }
    
    // Game completion tracking
    bool hasGameBeenCompleted() const { return gameCompleted; }
    void setGameCompleted(bool completed) { gameCompleted = completed; }
    
protected:
    ModelListener* modelListener;
    int tickCounter;
    int counter90to0;
    bool countingActive;
    
    // Game scores
    int leftPlayerScore;
    int rightPlayerScore;
    
    // Track if any game has been completed
    bool gameCompleted;
};

#endif // MODEL_HPP