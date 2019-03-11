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
    float topRowsAverageX(int rows);
    float botRowsAverageX(int rows);
    Point2i average();
    float area();
    float width();
    float height();
    float invSlope();
    float boundArea();
    Point2i operator [] (int i);
    void operator = (Blob arr);
    void operator += (Blob arr);
    int size();
    
private:
    void averageX();
    void averageY();
    void calcWidth();
    void calcHeight();
    void slope();
    void calculate();
    std::vector<Point2i> points;
    float aveX, aveY, wid, hei, iSlope;
    bool valsCalc;
};
