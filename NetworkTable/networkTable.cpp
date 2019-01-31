#include <iostream>
#include "networkTables/NetworkTable.h"


int main(int argc, char * argv[])
{
  
  double x = 1, y = 2, distance = 3;

  //nt::NetworkTableEntry xEntry, yEntry, Distance;
  nt::NetworkTable * table = NetworkTable::GetTable("datatable");

  int i = 1;

  while(true)
    {
      i++;
      x += i;
      y += i;
      distance += i;

      /*xEntry.setDouble(x);
      yEntry.setDouble(y);
      Distance.setDouble(distance);*/

      table->PutNumber("X", x);
      table->PutNumber("Y", y);
      table->PutNumber("Distance", distance);
    }

  return 0;
}
