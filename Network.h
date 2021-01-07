#pragma once

#include "Point.h"
#include "Model.h"
#include "Eigen/Dense"
#include "Functions.h"
#include "ErrorFeature.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
using namespace Eigen;
using namespace std;

class Network {
private:

public:

  //first point is useless
	vector<Point> knowns;

  //first point is useless
  vector<Point> unknowns;

  //correct size
  vector<Model> models;

  //vector of legs that cnnot exist
  vector<Model> invalid_legs;

	//Frequently used dimensions
	int n, u, r;

  //for max model generation
  int i;
  
	Network();

  /*
	Description:
  Initializes a Network object
  
	Input:
		string of filename

  Values Initialized:
    known with all of the points
	*/
	Network(string filename);

  /*
	Description:
  Checks the file title and assigned a format for program to read the file with
  
	Input:
		string str

  Values Initialized:
    int:
    1 for unknown coordinates
    2 for known coordinates
    3 for distance measurements
    4 for angle measurements
	*/
  void filetype(string str, int* type);

  /*
	Description:
  Iterates through the network and its values to display all
  
	Input:
		none

  Output:
    prints all values within the network
	*/
  void output();

  /*
	Description:
  Develops the Design Matrix for the given network
  
	Input:
		none

  Output:
    MatrixXd: returns the Desgin matrix 
	*/
  MatrixXd A();

  /*
	Description:
  returns the misclosure matrix
  W()
  
	Input:
		none

  Returns:
    MatrixXd of the misclosure matrix
	*/
  MatrixXd W();

  /*
	Description:
    deletes the first two column on the matrix
    to fix our pointer error
  
	Input:
		none

  Returns:
    MatrixXd with two less columns
	*/
  MatrixXd shrink(MatrixXd M);

  /*
  Description:
    Write a .csv file via a MatrixXd
    mat options: 
    --> "A" or "Design"
    --> "W" or "Misclosure"
    --> "S" or "Corrections"

  Input:
    String title: title of the matrix
    String mat: to determine how and where the matrix will look
  Output:
    A .txt file with 3 decimal precision
	*/
  void to_file(string title, string mat);

  /*
	Description:
  outputs the S or Correction matrix to a .txt file
  (because currently only symbols)
  
	Input:
		none

  Returns:
    none
	*/
  void S(string filename);

  /*
	Description:
  outputs all of the matrices. In order to not output one you must set their value to false
  
	Input:
		none unless setting a matrices value to false in order not to print

  Returns:
    outputs the values to a .txt file
	*/
  void file_all(bool A = true, bool W = true, bool S = true);

  /*
	Description:
  After the network of points has been read in it will output the optimal network for those given points and specifications
  
	Input:
		none

  Returns:
    none
	*/
  void OptomiseNetwork();

//we'll add this in a bit
void Combi(vector<vector<Model>>& options, vector<Model>& a, int reqLen, int s, int currLen, bool check[], int l);
  /*
	Description:
    Generate the Cx matrix
    Cx = (A^T * Cl^-1 * A)^T
  
	Input:
		none

  Returns:
    MatrixXd Cx
	*/
  MatrixXd Cx();

  /*
	Description:
    Generate the Cx matrix
    stddev^2 of each model
  
	Input:
		none

  Returns:
    MatrixXd Cl
	*/
  MatrixXd Cl();

  /*
	Description:
    Builds the base case of models

    Assumes that we will build the base case by going to every known point and taking measurements from them to ALL unknown points
  
	Input:
		none

  Returns:
    none
	*/
  void BuildBase();

  
  /*
	Description:
  Generates the B matrix used in computing stddev of ellipse error
  
	Input:
		none

  Returns:
    MatrixXd B
	*/
  MatrixXd B();

   /*
	Description:
    Generates the Cx_hat matrix to calculate stddev of measurements between measurements
  
	Input:
		none

  Returns:
    MatrixXd Cx_hat
	*/
  MatrixXd Cx_hat();

  /*
	Description:
   Generate the standard deviation matrix of the Cx_hat
   for use in calculating a, b, and bearing

   Formatted [[Ea],
              [Na],
              [Eb],
              [Nb]...]
  
	Input:
		none

  Returns:
    MatrixXd StdDev between each model measurement

	*/
  MatrixXd Ellipse_Stddev();

  /*
	Description:
   Iterates through the models matrix and returns a vector containing the indexes of models that use unknown measurements for both their to and from point.
  
	Input:
		none

  Returns:
    vector <int> unknonw_legs (x,1)
      x being the # of unknown legs
	*/
  vector<int> unknown_legs();

  /*
	Description:
    Calculates the error features a, b, bearing via the models that only consider unknown points on both ends
  
	Input:
		multiplier to multiplier a and b by

  Returns:
    vector<Error_Feature>: a vector containing pointers to the models and the components of their combined error features
	*/
vector<ErrorFeature> error_features(double multiplier = 2.45);

  /*
	Description:
    Generates all possible legs
  
	Input:
		multiplier to multiplier a and b by

  Returns:
    not sure...
	*/
  void generate_legs();

  /*
	Description:
    Checks to see if the combo already exists. Checks both order of points

    If from and to are the same returns true b/c it doesn't exist but should not be made
  
	Input:
		ID for the from point
    ID for the to point

  Returns:
    False if the combo does not exist
    True if the combo already exists
	*/
  bool combo_exists(string from, string to);

  /*
	Description:
    Checks to see if the combo already exists in the leg that was given

    If from and to are the same returns true b/c it doesn't exist but should not be made
  
	Input:
		ID for the from point
    ID for the to point

  Returns:
    False if the combo does not exist
    True if the combo already exists
	*/
  bool combo_exists(string from, string to, vector<Model> &models);

  /*
	Description:
    Outputs the current unknonw-unknown legs with a, b, bearing
  
	Input:

  Returns:
    
  */
  void output_error_legs();



/*
	Description:
    Iterates through all current combos and sees if the suggested set of models exists.
  
	Input:
    vector<vector<Model>> curr: vector of vector of models containing each possible network
    vector<Model> check: the set of models to check if it has been added to the vector of possible models

  Returns:
    False if the combo does not exist
    True if the combo already exists
  */
  bool combo_exists(vector<vector<Model>> &curr, vector<Model> &b);

  /*
	Description:
    Checks to see if the two sets of models contain all of and only the same models
  
	Input:
    vector<Model> a
    vector<Model> b

  Returns:
    False if the sets are unique from eachother
    True if the sets contain the same models
  */
  bool same(vector<Model> &an, vector<Model> &b);

  /*
	Description:
    Generates a vector of each possible combinations of unknown points
  
	Input:

  Returns:
    vector<Model> of all unknown leg combos
  */
  vector<Model> generate_unknown_legs();

  /*
	Description:
    Prints the more concise verion of the vector of models given
  
	Input:

  Returns:

  */
  void quick_print(vector<Model> &m, int n = -1);
  
  /*
	Description:
    Checks to see that the semi-major axis of each a relative confidence region is less than
  
  
	Input:
    double d: distance multiplier for error
    double c: constant added to the error
    double mult: multiplier to the semi-major axis to get to the desired relative confidence region

  Returns:
    outputs which legs passed and if the entire network passed
  */
  bool passes(double d=.000003, double c=.05, double mult=2.45);

/*
	Description:
    generates a list of each possible unknown-unknown combo
  
	Input:
    none

  Returns:
    string[] of all possible unknown-unknown combos
  */
  vector<string> combos();

/*
	Description:
    generates the mean of all the semi major axis
  
	Input:
    double multiplier = 2.45 of the alpha value

  Returns:
    double of the a value;
  */
  double average_a(double multiplier = 2.45);

  /*
	Description:
    generates the mean of all the semi major axis
  
	Input:
    double multiplier = 2.45 of the alpha value

  Returns:
    double of the a value;
  */
  vector<Model> max_legs();
  /*To Do:
  -Make function to generate all possible legs and leg combos
  -Figure out how to store results from each combo
  -Evaluate which combo is better
  -switch atan to atan2 in (Error_Feature: Breaing)
  */

/*
	Description:
    outputs grid distance of each point
  
	Input:
    

  Returns:
    
  */
  void output_grids();

  /*
	Description:
    reads in the scales for each point
  
	Input:
    

  Returns:
    
  */
  void establish_scales(string line);

  /*
	Description:
    reads in the scales for each point
  
	Input:
    

  Returns:
    
  */
  void model_scale(string line);

  /*
	Description:
  Returns correction vector (delta)
  S()
  
	Input:
		none

  Returns:
    MatrixXd of the correction vector matrix
	*/
  MatrixXd S();

  /*
	Description:
  Returns residuals vector
  
	Input:
		none

  Returns:
    MatrixXd of the correction vector matrix
	*/
  MatrixXd R();

  /*
	Description:
  Returns residuals vector
  
	Input:
		none

  Returns:
    MatrixXd of the correction vector matrix
	*/
  MatrixXd a_post();

  /*
	Description:
  Returns residuals vector
  
	Input:
		none

  Returns:
    MatrixXd of the correction vector matrix
	*/
  MatrixXd X_0();

    /*
	Description:
    Reduces to grid distance. Adjusts mean at the end
  
	Input:
		none

  Returns:
    none
	*/
  void reduce();
};