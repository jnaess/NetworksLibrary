#pragma once

#include "Functions.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class Point {
private:

public:

	//Point Label
  string ID;

  //Coordinates
  double E;
  double N;

  double delta_h;
  double H;
  double N_g;
  double R;
  double h;
  double h_prime;

  double Scale;
  //______________I guess later we can make these private. Not much benefit though..

  /*
	Description:
  Initializes a point object
  
	Input:
		none

  Values Initialized:
    ID = "null"
    E = -1
    N = -1
	*/
	Point();

  /*
	Description:
  Initializes a point object
  
	Input:
		string str of the line that is returns from a file.
    "ID E N"

  Values Initialized:
    ID = 
    E = 
    N = 
	*/
  Point(string str);

  /*
	Description:
    Outputs all components of the point
  
	Input:
    none

  Output:
    "Point: ID
    Easting: E, Northing: 
	*/
  void output();

  /*
	Description:
    returns the point's ID
  
	Input:
    none

  Output:
    string of the point ID
	*/
  string point_ID();

  /*
	Description:
    Generates the scale factor of the given point
  
	Input:
    double lat in DD
    double long in DD

  Output:
    none
	*/
  void scale(double lat, double lon);

  /*
	Description:
   Calculates earth's radius at the point
  
	Input:
    double lat in DD

  Output:
    none
	*/
  void calc_radius(double lat);
};