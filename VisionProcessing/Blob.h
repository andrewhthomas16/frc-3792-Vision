// Blob.h

#pragma once
#include <iostream>
#include <vector>
#include "Point2i.h"


class Blob
{
public:
    // Constructors
    Blob();
    Blob(int size);
    Blob(const Point2i point);
    Blob(const int x, const int y);
    Blob(const Point2i pointArr[], int size);
    Blob(const Blob & copy);
    
    // Deconstructor
    ~Blob();
    
    // Methods
    void addPoint(const int x, const int y);
    void addPoint(const Point2i & var);
    int averageX();
    int averageY();
    Point2i average();
    Point2i operator [] (int i);
    void operator = (Blob arr);
    void operator += (Blob arr);
	int size();
    
private:
    std::vector<Point2i> points;
};
