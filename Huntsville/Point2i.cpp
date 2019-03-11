// Point2i.cpp

#include "Point2i.h"


// Constructor
Point2i::Point2i(){}
Point2i::Point2i(int _x, int _y)
{
  x = _x;
  y = _y;
}
Point2i::Point2i(const Point2i & copy)
{
  x = copy.x;
  y = copy.y;
}

// Deconstructor
Point2i::~Point2i(){}


// Method to check if two Point2i objects are equal.
//Return true if they are equal.
bool Point2i::operator == (const Point2i & var)
{
  return (x == var.x && y == var.y);
}


// Method to check if two Point2i objects are not equal.
// Return true if they are not equal.
bool Point2i::operator != (const Point2i & var)
{
  return !(*this == var);
}


// Method to set this object equal to var.
void Point2i::operator = (const Point2i & var)
{
  x = var.x;
  y = var.y;
}
