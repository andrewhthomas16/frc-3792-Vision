// Blob.cpp


#include <iostream>
#include <vector>
#include "Blob.h"
#include "Point2i.h"


// Constructors
Blob::Blob()
{
    calcFalse();
}
Blob::Blob(int size)
{
    points.resize(size);
    
    calcFalse();
}
Blob::Blob(const Point2i point)
{
    points.resize(1);
    points.at(1) = point;
    
    calcFalse();
}
Blob::Blob(const int x, const int y)
{
    points.resize(1);
    points.at(1) = Point2i(x, y);
    
    calcFalse();
}
Blob::Blob(const Point2i pointArr[], int size)
{
    points.resize(size);
    for(int i = 0; i < size; i++)
        points.at(i) = pointArr[i];
    
    calcFalse();
}
Blob::Blob(const Blob & copy)
{
    points.resize(copy.points.size());
    for(int i = 0; i < copy.points.size(); i++)
        points.at(i) = copy.points.at(i);
    
    calcFalse();
}

// Deconstructor
Blob::~Blob(){}


// Method to add a Poin2i to pointsusing an x, y cordinate. valsCalc needs to be
// false to recalculate all the values.
void Blob::addPoint(const int x, const int y)
{
    points.push_back(Point2i(x, y));
    calcFalse();
}


// Method to add a Point2i to points using a Point2i object. valsCalc needs to be
// false to recalculate all the values.
void Blob::addPoint(const Point2i & var)
{
    points.push_back(var);
    calcFalse();
}


// Method to find the average x point between all the points.
void Blob::averageX()
{
    int avg = 0;
    for(int i = 0; i < points.size(); i++)
        avg += points.at(i).x;
    if(points.size() > 0)
    	aveX = avg / points.size();
    else
        aveX = -1;
}


// Method to find the average y point between all the points.
void Blob::averageY()
{
    int avg = 0;
    for(int i = 0; i < points.size(); i++)
        avg += points.at(i).y;
    if(points.size() > 0)
        aveY = avg / points.size();
    else
        aveY = -1;
}


// Method to find the width of a blob in pixels.
void Blob::calcWidth()
{
    int minX = 0, maxX = 0;
    
    for(int i = 1; i < points.size(); i++)
    {
        if(points[i].x < points[maxX].x)
            minX = i;
        if(points[i].x > points[minX].x)
            maxX = i;
    }
    
    if(points[maxX].y > points[minX].y)
        wid = points[maxX].x - points[minX].x;
    else
        wid = -1 * (points[maxX].x - points[minX].x);
 }


// Method to find the height of a blob in pixels.
void Blob::calcHeight()
{
    float minY = -1, maxY = -1;
    
    for(int i = 0; i < points.size(); i++)
    {
        if(points[i].y < minY || minY == -1)
            minY = points[i].y;
        if(points[i].y > maxY || maxY == -1)
            maxY = points[i].y;
    }
    
    hei = maxY - minY;
}


// Method to find the inverse slope.
void Blob::slope()
{
    float minX = -1, maxX = -1, lastAveX = -1, lastAveY = -1, aveX, aveY;
    int num = 0;
    std::vector<std::vector<int>> p;
    
    for(int i = 0; i < points.size(); i++) // Find min and max vals.
    {
        if(points[i].y < minX || minX == -1)
            minX = points[i].y;
        if(points[i].y > maxX || maxX == -1)
            maxX = points[i].y;
    }
    
    p.resize(maxX - minX);
    
    for(int x = 0; x < p.size(); x++) // Add all the points the the respective
    {                                 // x values/colunms.
        for(int i = 0; i < points.size(); i++)
        {
            if(points[i].y == x + minX)
                p[x].push_back(points[i].x);
        }
    }
    
    for(int x = 0; x < p.size(); x++)
    {
        for(int y = 0; y < p[x].size(); y++) // Find the average Y for an x value.
        {
            aveY += p[x][y];
            aveX = x + minX;
        }
        aveY /= p[x].size();
        
        if(lastAveX != -1) // Find the slope if it is not the first value.
            iSlope += (aveY - lastAveY) / (aveX - lastAveX);
        
        lastAveX = aveX;
        lastAveY = aveY;
        num++;
    }
    
    iSlope /= num; // Find the average slope.
}


//Function to see if values need to be recalculated.
/*void Blob::calculate()
{
    if(valsCalc)
        return;
    else
    {
        averageY();
        averageX();
        calcWidth();
        calcHeight();
        
        valsCalc = true;
    }
}*/


// Method to return the average x of the num rows near the top of a blob.
float Blob::topRowsAverageX(int rows)
{
    float minY = -1, ave, numPoints;
    
    for(int i = 0; i < points.size(); i++) // Find the min y.
    {
        if(points[i].y < minY || minY == -1)
            minY = points[i].y;
    }
    
    if(minY == -1)
        return -1;
    else
        for(int i = 0; i < points.size(); i++) // With the min y find the average x
            if(points[i].y >= minY + rows)     // of the rows between min y and rows.
            {
                ave += points[i].x;
                numPoints++;
            }
    
    return ave / numPoints;
}


// Method to return the average x of the num rows near the bottom of a blob.
float Blob::botRowsAverageX(int rows)
{
    float maxY = -1, ave, numPoints;
    
    for(int i = 0; i < points.size(); i++) // Find the max y.
    {
        if(points[i].y > maxY || maxY == -1)
            maxY = points[i].y;
    }
    
    if(maxY == -1)
        return -1;
    else
        for(int i = 0; i < points.size(); i++) // With the max y find the average x
            if(points[i].y >= maxY - rows)     // of the rows between rows and max y.
            {
                ave += points[i].x;
                numPoints++;
            }
    
    return ave / numPoints;
}


// Method to return the average Point2i in all points.
Point2i Blob::average()
{
    if(calcAveX && calcAveY)
        return Point2i(aveX, aveY);
    else
    {
        if(!calcAveX)
            averageX();
        if(!calcAveY)
            averageY();
        
        return Point2i(aveX, aveY);
    }
}


// Method to return the width in pixels.
float Blob::width()
{
    if(calcWid)
        return wid;
    else
    {
        calcWidth();
        return wid;
    }
}


// Method to return the height in pixels.
float Blob::height()
{
    if(calcHeig)
        return hei;
    else
    {
        calcHeight();
        return hei;
    }
}


// Method to return the inverse slope.
float Blob::invSlope()
{
    if(calcSlope)
        return iSlope;
    else
    {
        slope();
        return iSlope;
    }
}


// Method to return the area of the blob in pixels.
float Blob::area()
{
    return points.size();
}


// Method to return the area of a rectangle around a blob.
float Blob::boundArea()
{
    return width() * height();
}


// Return a Point2i object in the points vector at i.
Point2i Blob::operator [] (int i)
{
    if(i < points.size())
        return points[i];
    
    return Point2i(-1, -1);
}


// Operator to set this to arr.
void Blob::operator = (Blob arr)
{
    points = arr.points;
    aveX = arr.aveX;
    aveY = arr.aveY;
    wid = arr.wid;
    hei = arr.hei;
    calcFalse();
}


// Operator to set this to this + arr.
void Blob::operator += (Blob arr)
{
	for(int i = 0; i < arr.points.size(); i++)
		points.push_back(arr.points.at(i));
    calcFalse();
}


// Method to return the size of the points vector.
int Blob::size()
{
    return points.size();
}


// Method to set all calculations to false. This means that
// all these values will be calculated again.
void Blob::calcFalse()
{
    calcAveX = false;
    calcAveY = false;
    calcWid = false;
    calcHeig = false;
    calcSlope = false;
}
