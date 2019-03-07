// LookUp.cpp

#include <iostream>
#include <vector>
#include <fstream>
#include "LookUp.h"


LookUp::LookUp(std::string fileName) // Constructor
{
    std::ifstream in(fileName);
    if(!in)
    {
        std::cout << "Cannot open the file: " << fileName << std::endl;
        return;
    }
    
    std::string str;
    
    while(std::getline(in, str))
    {
        for(int i = 0; i < str.size(); i++)
            if(str.substr(i, 1).compare(",") == 0)
            {
                pointsX.push_back(std::stoi(str.substr(0, i)));
                pointsY.push_back(std::stoi(str.substr(i + 2, str.size() - i - 2)));
            }
    }
    
    in.close();
    
    float temp;
    
    for(int i = 0; i < pointsX.size(); i++)
        for(int j = 0; j < pointsX.size() - 1; j++)
            if(pointsX.at(j) < pointsX.at(j + 1))
            {
                temp = pointsX.at(j); // Sort x component
                pointsX.at(j) = pointsX.at(j + 1);
                pointsX.at(j + 1) = temp;
                
                temp = pointsY.at(j); // Sort y component
                pointsY.at(j) = pointsY.at(j + 1);
                pointsY.at(j + 1) = temp;
            }
    
    return;
}


LookUp::~LookUp() {} // Deconstructor


// Get the x value of the look up table. 0.0 is
// the default return value.
float LookUp::getVal(float x) const
{
    for(int i = 0; i < pointsX.size(); i++)
        if(pointsX[i] == x)
            return pointsY[i];
        else if(i < pointsX.size() - 1 && pointsX[i] < x && pointsX[i + 1] > x)
            return closestVal(x, pointsX[i], pointsX[i + 1], pointsY[i], pointsY[i + 1]);
    
    return 0.0;
}


float LookUp::closestVal(float x, float x1, float x2, float y1, float y2) const
{ // Get the closest y value to x.
    return y1 + (y2 - y1) * (x - x1) / (x2- x1);
}
