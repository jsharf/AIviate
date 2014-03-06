#include <math.h>

#define SOLVE_ON_PUSH 0
#define SOLVE_ON_PULL 1

// http://en.wikipedia.org/wiki/Trapezoidal_rule
double trap(int *data, int a, int b, int n);

// http://en.wikipedia.org/wiki/Romberg%27s_method
int romberg(int *data, int n, double (*quad)(int*, int, int, int));

template <class T>
class Smoother
{
private:
    T* rawValues;                   // Array to store raw values pushed by the user
    unsigned int rawValuesLength;   // The length of the rawValues array
    unsigned int valueIndex;        // The index to push a new raw value to
    bool loaded;                    // Has the rawValues array been fully filled yet?
    unsigned char solveTime;        // When to solve for the running average
    T average;                      // The stored running average
    
    inline void calculateAverage(void)
    {
        average = (T)0;
        unsigned int i;
        for(i = 0; i < (loaded?rawValuesLength:valueIndex); i++)
        {
            average += rawValues[i];
        }
        average /= (T)i;
    }
    
public:
    Smoother(unsigned int size, unsigned char _solveTime = SOLVE_ON_PULL)
    {
        rawValues = new T[size];    // Allocate the rawValues array of the specified type and specified size
        rawValuesLength = size;     // Set the length variable
        valueIndex = 0;             // Raw value pushes will start at the beginning of the array
        loaded = false;             // The array has not been filled yet
        average = (T)0;             // Averaged value is 0, for now
        solveTime = _solveTime;     // By default, solve for the average when pulling a value
    }
    
    void pushValue(T rawValue)      // 
    {
        rawValues[valueIndex] = rawValue;
        
        if(solveTime == SOLVE_ON_PUSH)
        {
            calculateAverage();
        }
        
        valueIndex++;
        if(valueIndex == rawValuesLength)
        {
            loaded = true;
            valueIndex = 0;
        }
    }
    
    T pullValue(void)
    {
        if(solveTime == SOLVE_ON_PULL)
        {
            calculateAverage();
        }
        
        return average;
    }
};