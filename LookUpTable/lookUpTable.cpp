// lookUpTable.cpp

#include <iostream>
#include <vector>
#include <fstream>


int main(int argc, char ** argv)
{
    std::vector<float> pointsX;
    std::vector<float> pointsY;
    std::string fileName = "look.txt";
    std::ifstream in(fileName);
    
    if(!in)
    {
        std::cout << "Cannot open the file: " << fileName << std::endl;
        return 1;
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
    
    for(int i = 0; i < pointsX.size(); i++)
        std::cout << "(" << pointsX.at(i) << ", " << pointsY.at(i) << ")" << std::endl;
    
    return 0;
}
