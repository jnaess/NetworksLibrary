#pragma once

#include "Point.h"
#include "Model.h"
#include "Eigen/Dense"
#include "Functions.h"
#include "ErrorFeature.h"
#include "Network.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <cstdio>
using namespace Eigen;
using namespace std;

class Network_Optomiser {
private:

public:

  //first point is useless
	string base;

  //number of combos that have been made
  int combo = 0;

  //one network per combo
  vector<Network> networks;

  Network min;

  Network_Optomiser();

  /*
	Description:
  Initializes a Network object
  
	Input:
		string of filename

  Values Initialized:
    known with all of the points
	*/
	Network_Optomiser(string filename);

  /*
	Description:
    used by generate_combos to generate each combination of a certain length quota
  
	Input:
		string a[], 
    int reqLen, 
    int s, 
    int currLen, 
    bool check[], 
    int l

  Output:
    One text file per combination
    
	*/
  void Combi(vector<string>, int reqLen, int s, int currLen, bool check[], int l);

/*
	Description:
    Used to generate all possible combinations of networks and adds the base case

	Input:
		none lol

  Output:
    One text file per combination
    
	*/
  void generate_combos();

  /*
	Description:
    Removes all txt file combos

	Input:
		int n: the number of combo_x.txt files that exist
    (-1 if would would prefer to use the combo class value)

  Output:
    none
    
	*/
  //remove() does not seem to be importing
  //void remove_combos(int n = -1);

  /*
	Description:
    Generates all possible networks

	Input:
		
  Output:
    each one is placed within networks
    
	*/
  void generate_networks();


  /*
	Description:
    Iterates through all networks and picks the one with the lowest legs that meets requirements

    Tie breaker given to the lower average mean

	Input:
		
  Output:
    each one is placed within networks
    
	*/
  void best_network();

  /*
	Description:
    returns the index of the lowest value

	Input:
		vector<int> legs: specifically of the number of legs in each successful network

  Returns:
    int <0 if the lowest index value was singular and found


    int -1 if there were more than one of the same lowest values
    ^^ not yet implemented
    
	*/
  int lowest(vector<int> legs);
};