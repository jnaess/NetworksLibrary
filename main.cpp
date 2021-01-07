/*
This template contains version 3.3.7 of the Eigen matrix library found here: http://eigen.tuxfamily.org. It also contains some sample code that you can use for testing before replacing it with your own code. The idea is that you can create a fork of this template to avoid having to set things up yourself each time you want to use Eigen.
*/

// Include required source files:

#include <iostream>
#include <cmath>
#include "Eigen/Dense"
#include "Network.h"
#include "Model.h"
#include "Point.h"
#include "Functions.h"
#include "ErrorFeature.h"
#include "Network_Optomiser.h"

// Bring namespaces and key functions into current scope:

using namespace Eigen;
using namespace std;
using Eigen::MatrixXd;

// Declare the main program function:

int main() {
  
//__________Part 2 Step 1______________________________
  Network n = Network("part_two.txt");

  n.output();
  n.reduce();

  





//__________Part 3 Step 1______________________________

  Network net = Network("adjust_network.txt");

  //The adjusted observations
  net.reduce();

  //output of all values
  net.output();

  //The residuals
  //Misclosure
  TestPrint(net.W(), "Misclosure");
  //Correction vector (delta)
  TestPrint(net.S(), "Correction Vector");
  //Residuals
  TestPrint(net.R(), "Residuals");

  //The corrections to the approximate parameters
  TestPrint(net.S(), "Correction Vector");

  //The final estimated parameters
  TestPrint(net.X_0() + net.S(), "Final Estimated Parameters");

  //The elements of the variance-covariance matrix of the estimated parameters
  TestPrint(net.Cx(), "Variance-Covariance Matrix");

  //The a-posteriori variance factor
  TestPrint(net.a_post(), "A-Posteriori");

  //The 95% point and relative confidence region parameters
  net.output_error_legs();






//__________Part 3 Step 2______________________________

  //Legs included: 3 -> 4, 6 -> 4, 4 -> 5
   Network fin = Network("final_network.txt");

  //The adjusted observations
  fin.reduce();

  //output of all values
  fin.output();

  //The 95% point and relative confidence region parameters
  fin.output_error_legs();
}