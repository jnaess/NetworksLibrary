#pragma once
#include "Eigen/Dense"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace Eigen;
using namespace std;

/*
Description:
	Used to test that a MatrixXd has been made correctly
Input:
	MatrixXd M: that will be printed
	String title: the desirect matrix's title
Output:
	Print to the screen of the matrix and its respective title on top
*/
void TestPrint(MatrixXd M, string title);

/*
Description:
	Enter the degree, minute, second angle
  returns the angle in degrees
Input:
	double deg, Degrees
  double min, Minutes
  double sec, Seconds

Output:
	double of the angle in degree decimals
*/
double degrees(double deg, double min, double sec);

/*
Description:
	Enter the degree, minute, second angle
  returns the angle in degrees
Input:
	double degrees, The degrees in decimal degree

Output:
	vector of doubles
  [0] --> Degrees
  [1] --> Minutes
  [2] --> Seconds
  */
vector<double> dms(double degrees);

/*
Description:
	Convert radians to decimal degrees
Input:
  double rad, radians

Output:
	double of the angle in degree decimals
*/
double degrees(double rad);

/*
Description:
	Write a .txt file via a MatrixXd
Input:
	A String for :filename
	A MatrixXd to read off
Output:
	A .txt file with x decimal precision
*/
void Output(string filename, MatrixXd m);

/*
Description:
	return decimal degrees to radians
Input:
	decimal degrees
Output:
  radians
*/
double radians(double deg);