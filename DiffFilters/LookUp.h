// LookUp.h

#pragma once
#include <vector>
#include <fstream>


class LookUp
{
public:
    LookUp(std::string fileName); // Constructor
    ~LookUp(); // Deconstructor
    
    float getVal(float x) const;
    
private:
    float closestVal(float x, float x1, float x2, float y1, float y2) const;
    std::vector<float> pointsX;
    std::vector<float> pointsY;
};
