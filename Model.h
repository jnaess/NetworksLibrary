#pragma once

#include "Point.h"
#include "Functions.h"
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#define PI 3.14159265;

class Model {
private:


public:
	//Model Type
  // "distance" | "angle"
  string type;

  //Pointers to the start and end points
  Point *from;
  Point *to;

  //Only for angle models
  Point *at;

  //Measurement taken
  //distance --> m
  //angle --> degrees
  double meas;

  //standard deviation of a measurement that will be taken
  double stddev;

  //middle point scale 
  double middle_k=-1;
  /*
	Description:
  Initializes default contructor
  
	Input:
		none

  Values Initialized:
    type = "unassigned"
	*/
	Model();

  /*
	Description:
  Initializes the linear model
  
	Input:
		int t: 1 for linear, 2 for angle
		string line: of the line being read 
    fomatted: 
    for distance --> From To Measurement
    for angle --> At From To Measurement
    vector<Point> knowns: of the points to set up the correct pointer
    vector<Point> unknowns: of the points to set up the correct pointers

  Values Initialized:
    type = "linear"
    initializes a point for "from"
    initializes a point for "to"
    initializes meas as the distance given
	*/
	Model(int t, string line, vector<Point> &knowns, vector<Point> &unknowns);
    
  /*
	Description:
  Finds the placeholder of a point based off of the provided ID
  
	Input:
		none

  Returns:
    int of the index
    -1 if no ID was matched
	*/
  int find_placeholder(string ID, vector<Point> &points);

  /*
	Description:
  Picks which model to output from
  
	Input:
		none

  Returns:
    none
	*/
  void output();

  /*
	Description:
  Outputs a distance model
  
	Input:
		none

  Returns:
    none
	*/
  void output_distance();

  /*
	Description:
  Outputs a angle model
  
	Input:
		none

  Returns:
    none
	*/
  void output_angle();

  /*
	Description:
  Returns the partial derivative the from point being derived for the distance model
  
	Input:
		string axis to determine if we want Easting or Northing partial

  Returns:
    double of the correct partial
	*/
  double distance_from(string axis);

  /*
	Description:
  Returns the partial derivative the to point being derived for the distance model
  
	Input:
		string axis to determine if we want Easting or Northing partial

  Returns:
    double of the correct partial
	*/
  double distance_to(string axis);

    /*
	Description:
  Returns the partial derivative the from point being derived for the angle model
  
	Input:
		string axis to determine if we want Easting or Northing partial

  Returns:
    double of the correct partial
	*/
  double angle_from(string axis);

  /*
	Description:
  Returns the partial derivative the to point being derived for the angle model
  
	Input:
		string axis to determine if we want Easting or Northing partial

  Returns:
    double of the correct partial
	*/
  double angle_to(string axis);

  /*
	Description:
  Picks the model to calc from and then determines the approx measurement from given coordinates
  
	Input:
		none

  Returns:
    double of the current approx distance
	*/
  double meas_approx();

  /*
	Description:
  Looks at the ID and determines if it exsists and which partical derivatiev that entails
  
	Input:
		string ID: of the desired point for the partial
    string axis: either "N" or "E" for which partial

  Returns:
    double current partial
	*/
  double part(string ID, string axis);

  /*
	Description:
  Picks the model to calc from and then determines the approx measurement from given coordinates
  
	Input:
		none

  Returns:
    double of the current approx measurment
	*/
  double meas_estimate();

  /*
	Description:
    Initializes the standard deviation of the measurement
    based off of currect coordinates (unknown or known)
  
	Input:
		double std: is the given standard deviation for each measurment
    double ppm: is the additional std deviation to be calculated onto each measurement

  Returns:
    none
	*/
  void set_stddev(double std = 0, double ppm = 0);

  /*
	Description:
    prints the concise version of the model
  
	Input:
		none

  Returns:

	*/
  void quick_print();

  /*
	Description:
    Calculates the geotedic distance between the two points
  
	Input:
		none

  Returns:

	*/
  double D2();

  /*
	Description:
    Calculates the mark-to-mark distance between the two points

  
	Input:
		none

  Returns:

	*/
  double D1();

  /*
	Description:
    Calculates the ellispoid chord length

  
	Input:
		none

  Returns:

	*/
  double D3();

/*
  Description:
    Calculates the scale between two distance points


  Input:
		

  Returns:
    the scale Kge
	*/
  double scale();

  /*
  Description:
    Calculates the ke


  Input:
		

  Returns:
    the scale Kes
	*/
  double ke();

/*
  Description:
    Calculates the scale of a point


  Input:
		

  Returns:

	*/
  double middle_scale(double lat, double lon);

/*
  Description:
    returns the grid distance as calculated with scale and measurement given

  Input:
		

  Returns:

	*/
  double Dg();


/*
  Description:
    Prints all values that will be needed in order to generate the three distance and scale values for reductions

  Input:
		none

  Returns:
    none
	*/
  void scale_generating_output();
};