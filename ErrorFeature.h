#pragma once

#include "Point.h"
#include "Model.h"
#include "Functions.h"

#include <string>
#include <vector>
#include <cmath>
using namespace std;

#define PI 3.14159265;

class ErrorFeature {
private:

public:
	//The two pointers to models included in this error calculation
  Model * model;

  //double of major and minor axis of elliptical error
  double a;
  double b;

  //bearing of elliptical error
  //--> degrees
  double bearing;

  /*
	Description:
  Initializes default contructor
  
	Input:
		none

  Values Initialized:
    does nothing
	*/
	ErrorFeature();

  /*
	Description:
  Initializes the three numerical components of the error feature object
  
	Input:
		double e: easting stddev of model
    double n: northing stddev of model
    Model mode: the model to have a pointer attached
    double mult: the multiplier to be applied to a and b values

  Values Initialized:
    a, b, bearing (degrees), and the pointer to the model
	*/
	ErrorFeature(double e, double n,  double d, Model *mode, double mult = 2.45);

    /*
	Description:
    Calculates the a axis of elliptical error
  
	Input:
		double e: the stddev of the easting
    double n: the stddev of the northing
    double multiplier: the multiplier to be applied to the major axis to account for desired confidence

  Returns:
    none
	*/
  void A(double e, double n, double d,  double multiplier);

  /*
	Description:
    Calculates the b axis of elliptical error
  
	Input:
		double e: the stddev of the easting
    double n: the stddev of the northing
    double multiplier: the multiplier to be applied to the major axis to account for desired confidence

  Returns:
    none
	*/
  void B(double e, double n, double d,  double multiplier);

  /*
	Description:
    Calculates the bearing B shift of elliptical error
  
	Input:
		double e: the stddev of the easting
    double n: the stddev of the northing

  Returns:
    none
	*/
  void Bearing(double e, double n, double d );

/*
	Description:
    Standard print of the error feature following it's model
  
	Input:
		double e: the stddev of the easting
    double n: the stddev of the northing

  Returns:
    none
	*/
  void output();
};