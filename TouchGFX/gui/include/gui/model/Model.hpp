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
    
protected:
    ModelListener* modelListener;
    int tickCounter;
    int counter90to0;
    bool countingActive;
};

#endif // MODEL_HPP
