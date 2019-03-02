// Point2i.h

#pragma once


class Point2i
{
 public:
  // Constructors
  Point2i();
  Point2i(const int x, const int y);
  Point2i(const Point2i & copy);
    
  // Deconstructor
  ~Point2i();
    
  // Methods
  bool operator == (const Point2i & var);
  bool operator != (const Point2i & var);
  void operator = (const Point2i & var);
    
  int x, y;
};
