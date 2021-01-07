#include "Network.h"

#include <sstream>

Network::Network() {
  return;
}

Network::Network(string filename) {
  
  //creating two first points in the vectors to try and fix the pointer issues. erased at the end of this function
  unknowns.emplace_back("-1 1 1 1 1 1 1");
  knowns.emplace_back("-2 1 1 1 1 1 1");

  ifstream file(filename);

  if (file.is_open()) {
      string line;

      //these are used to compare the type of input to read
      int type = -1;

      while (getline(file, line)) {
        //Update the filetype
        //If it switches value then another line needs to be read
        int prev = type;
        filetype(line, &type);
        if(type == prev){

          //This means that we can use this line to input some sort of value
          if(type == 1){
            //unknown_coords
            unknowns.emplace_back(line);
          }

          else if(type == 2){
            //known_coords
            knowns.emplace_back(line);
          }

          else if(type == 3){
            //distance
            models.emplace_back(1, line, knowns, unknowns);
          }

          else if(type == 4){
            //angle
            models.emplace_back(2, line, knowns, unknowns);
          }

          else if(type == 5){
            //invalid_leg
            invalid_legs.emplace_back(1, line, knowns, unknowns);
          }
          else if(type == 6){
            //establish scale
            establish_scales(line);
          }
          else if(type == 7){
            //calculate the middle scale of a point
            model_scale(line);
          }
        }
      }
      file.close();
  }
  u = unknowns.size()*2-2;
  n = r = models.size();
}

void Network::filetype(string str, int* type){
  //"unknown_coords" --> 1
  if(str == "unknown_coords")
    *type = 1;

  //"known_coords" --> 2
  else if(str == "known_coords")
    *type =  2;

  //"distance" --> 3
  else if(str == "distance")
    *type =  3;

  //"angle" --> 4
  else if(str == "angle")
    *type =  4;

  //"invalid_legs" --> 5
  else if(str == "invalid_legs")
    *type =  5;

  //"establish scale for points" --> 6
  else if(str == "establish_scale")
    *type =  6;

  //"establish scale for model middles" --> 7
  else if(str == "model_scale")
    *type =  7;
  
  //this is data that should be inputted
  else return;
}

void Network::output(){
  //Iterate through all unknown points
  cout << "_______Unknown Points_______" << endl;

  //avoid the first fake point
  for(int i=1; i<unknowns.size(); i++){
    unknowns[i].output();
  }

  cout << endl;

  //Iterate through all unknown points
  cout << "_______Known Points_______" << endl;

  //avoid the first fake point
  for(int i=1; i<knowns.size(); i++){
    knowns[i].output();
  }

  cout << endl;

  //Iterate through all unknown points
  cout << "_______Linearized Models_______" << endl;

  for(int i=0; i<models.size(); i++){
    models[i].output();
  }

  cout << endl;

  
	//Frequently used dimensions
	cout << "___Dimensions___" << endl;
  cout << "u: " << u << endl;
  cout << "n: " << n << endl;
  cout << "r: " << r << endl;

  cout << endl;
}

MatrixXd Network::A(){
  //Order is determined by the order of the unknown point vector
  //update values

  MatrixXd A = MatrixXd(models.size(), unknowns.size()*2);

  int col = 0;

  for(int i=0; i<unknowns.size();i++){
    //iterate through all models
    for(int j=0; j<models.size();j++){
      //Easting
      A(j,col) = models[j].part(unknowns[i].ID,"E");
    }

    //Northing Column
    col++;
    for(int j=0; j<models.size();j++){
      //Northing
      A(j,col) = models[j].part(unknowns[i].ID,"N");
    }

    //Next Easting Column
    col++;
  }
  return shrink(A);
}

MatrixXd Network::W(){
  MatrixXd W = MatrixXd(n,1);
  //iterate through all models
    for(int i=0; i < models.size(); i++){
      //observation - estimate
      if (models[i].type == "distance"){
        W(i,0) = models[i].meas - models[i].meas_estimate();
      }
      else if (models[i].type == "angle"){
        W(i,0) = models[i].meas + models[i].meas_estimate();
      }
    }

    return W;
}

MatrixXd Network::shrink(MatrixXd M){
  //declare a matrix with two less columns
  MatrixXd F(M.rows(),M.cols()-2);
  
  //start at the third column j=2
  for( int i=0; i<M.rows();i++){
    for(int j=2; j<M.cols();j++){
      F(i,j-2) = M(i,j);
    }
  }

  return F;
}

void Network::to_file(string title, string mat){
  if(mat == "A" || mat == "Design"){
    //Output design matrix via the functions.cpp method
    Output(title, A());
  }

  else if(mat == "W" || mat == "Misclosure"){
    //Output misclosure matrix via the functions.cpp method
    Output(title, W());
  }

  else if(mat == "S" || mat == "Corrections"){
    //Output correction matrix via the S() in Newtork
    S(title);
  }
}

void Network::S(string filename){
  //Output file
	ofstream out(filename);

  //check for failure
	if (out.fail())
	{
		cout << "Could not open output file" << endl;
		exit(1);
	}

	out << filename << endl;

  //iterate through the known points
  //avoid the first fake point
		for (int i=1; i<unknowns.size(); i++) {
      //Easting and Northing attached to each point
      out << unknowns[i].ID << "E" <<endl;
			out << unknowns[i].ID << "N" << endl;
		}

	return;
}

void Network::file_all(bool A, bool W, bool S){
  if(A)
    to_file("A_Matrix.txt", "A");

  if(W)
    to_file("W_Matrix.txt", "W");

  if(S)
    to_file("S_Matrix.txt", "S");

  return;
}

void Network::OptomiseNetwork(){
  //build base set
  //the base set will be the first models added onto all network options
  vector<vector<Model>> options;

  //vector of unknowns combos (accounts for impossible legs)
  vector<Model> unknown_legs = generate_unknown_legs();
  int i,n;

  //this should be vector of possible models
  n = unknown_legs.size();
  bool check[n];

  //set all booleans to false
  for(i = 0; i < n; i++){
    check[i] = false;
  }


  for(i = 1; i <= n; i++) {    
    Combi(options, unknown_legs, i, 0, 0, check, n);
   }
  //check stddev of unknown legs

  //add new leg if needed

  return;
}

void Network::Combi(vector<vector<Model>>& options, vector<Model>& a, int reqLen, int s, int currLen, bool check[], int l)
{
  //do not add more here
  if(currLen > reqLen){
    return;
  }

  //this works so add the model
  else if (currLen == reqLen) {
    //this should build the first set here to be just from known to unknowns
    vector<Model> temp = models;

    //output each unique combination
    for (i = 0; i < l; i++) {
      if (check[i] == true) {
        cout << "c" << endl;
        //add this model to the set
        temp.push_back(a[i]);
      }
    }

    //done with this series... add it to the models then make a new one
    if(!combo_exists(options, temp)){
      options.push_back(temp);
    }

    return;
    }

  //once the start length is the same as the vector length then we need to cut this loop
  if (s == l) {
    return;
  }

  //start length was not vector length so next index can be set as true
  check[s] = true;
  //increment the desired current length of the set and start length
  Combi(options, a, reqLen, s + 1, currLen + 1, check, l);

  //set that start point to not be checked
  check[s] = false;
  //append all desired models to the vector
  Combi(options, a, reqLen, s + 1, currLen, check, l);
}

MatrixXd Network::Cx(){
  return (A().transpose()*Cl().inverse()*A()).inverse();
}

MatrixXd Network::Cl(){
  n = models.size();

  //Initialize correct size of matrix
  MatrixXd cl = MatrixXd(n, n).setZero();

  for(int i=0; i<n; i++){
    //set the diagonal to the variance
    cl(i,i) = models[i].stddev*models[i].stddev;
  }

  return cl;
}

void Network::BuildBase(){
  //skip first fake point
  for(int i = 1; i<knowns.size();i++){
    //from point
    string from = knowns[i].ID;

     for(int j = 1; i<unknowns.size();j++){
       //to point
       string to = unknowns[j].ID;

       //type 1 for distance model
       // -1 for the measurement because this is before measureing
       string line = from + "  " + to + "-1";

       //initialize this new model
       models.emplace_back(1, line, knowns, unknowns);
     }
  }
}

MatrixXd Network::B(){
  vector<int> legs = unknown_legs();
  int u = 8;
  int n = models.size();

  //initialize correct size of B and initialize all placeholders to setZero
  MatrixXd B = MatrixXd(n*2,u).setZero();

  //initialize these so that they are not constantly remade
  string from;
  string to;
  int col, row;

  //start filling values
  for(int i=0; i<models.size(); i++){
    //assign values from easier reading of code
    from = models[i].from->ID;
    to = models[i].to->ID;

    //returns -1 if point does not exsist in unknowns vector
    //otherwise returns placeholder for point
    //__________from point______________
    col = models[i].find_placeholder(from, unknowns);

    //check from
    if( col != -1){
      //then point is an unknown
      //assign correct column indexes

      //Easting partial
      row = i*2;
      //Easting partial of 1
      B( row, col * 2 - 2 ) = 1;

      //northing partial
      row = i*2+1;
      //northing partial of 1
      B( row, col * 2 - 1 ) = 1;
    }

    //__________to point____________________
    col = models[i].find_placeholder(to, unknowns);

  //check from
    if( col != -1){
      //then point is an unknown
      //assign correct column indexes

      //Easting partial
      row = i*2;
      //Easting partial of -1
      B( row, col * 2 - 2 ) = -1;

      //northing partial
      row = i*2+1;
      //northing partial of -1
      B( row, col * 2 - 1 ) = -1;
    }
  }
  return B;
}

MatrixXd Network::Cx_hat(){
  //This was declared to cut down function processing time
  MatrixXd b = B();

  return b * Cx() * b.transpose();
}

MatrixXd Network::Ellipse_Stddev(){
  //declare the ellipse matrix
  MatrixXd Ellip_std = MatrixXd(models.size()*2,2);

  //temp store the Cx_hat matrix
  MatrixXd cx = Cx_hat();

  //cols == rows and only the diagonal matter
  for(int i = 0; i < cx.cols(); i++){

    if (i % 2 == 0){
      //indicated Easting
      Ellip_std(i,0) = sqrt(cx(i,i));

      //left first off fiagonal
      Ellip_std(i,1) = cx(i+1,i);
    }
    else if (i % 2 == 1){
      //indicated Northing stddev
      Ellip_std(i,0) = sqrt(cx(i,i));

      //right first off diagonal
      Ellip_std(i,1) = cx(i-1,i);
      
    }
  }

  return Ellip_std;
}

vector<int> Network::unknown_legs(){
  vector<int> legs;

  for(int i = 0; i < models.size(); i++){
    //Check if from point is in unknowns
    if (models[i].find_placeholder(models[i].from->ID, unknowns) != -1){
      //Check if to point is in unknowns
      if (models[i].find_placeholder(models[i].to->ID, unknowns) != -1){
        //store the model's index
        legs.emplace_back(i);
      }
    }
  }

  return legs;
}

vector<ErrorFeature> Network::error_features(double multiplier){
  //array of stddev for each model
  MatrixXd stddev = Ellipse_Stddev();
  //vector of indexes to set pointers and obtain from stddev
  vector<int> legs = unknown_legs();
  //initialize error vector
  vector<ErrorFeature> errors;

  //so that these do not need to be made over and over
  //j is the model vector index to obtain these values from
  double e, n, j, d;

  for(int i = 0; i < legs.size(); i++){
    //initialize each error feature
    //need e, n, multiplier

    //this is the model index location
    j = legs[i];

    //these are the respective e and n error for that model
    e = stddev(j*2,0);
    n = stddev(j*2+1,0);
    d = stddev(j*2,1);
    errors.emplace_back(e, n, d, &models[j], multiplier);
  }

  return errors;
}

void  Network::generate_legs(){
  //this is used for declaring each model
  string line;
  //skip first fake point
  for(int i = 1; i < knowns.size(); i++){
    //cout << knowns[i].ID << endl;

    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it
;
      if( !combo_exists(knowns[i].ID, unknowns[j].ID) ){
        //create string to initialize the model
        
        line = knowns[i].ID + "   " + unknowns[j].ID + "   -1";

        //create model
        models.emplace_back(1, line, knowns, unknowns);
      }
    }
  }

  //skip first fake point
  for(int i = 1; i < unknowns.size(); i++){
    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it

      if( !combo_exists(unknowns[i].ID, unknowns[j].ID) ){

        //create string to initialize the model
        line = unknowns[i].ID + "   " + unknowns[j].ID + "   -1";


        //create model
        models.emplace_back(1, line, knowns, unknowns);
      }
    }

    //known combos were already tested
  }
}

bool Network::combo_exists(string from, string to){
  //If from and to are the same returns true b/c it doesn't exist but should not be made
  if (from == to){
    return true;
  }

  //check all models for combo existance
  for(int i = 0; i < models.size(); i++){
    //itterate through each model

    if(models[i].from->ID == from && models[i].to->ID == to){
      return true;
    }
    //check to --> from
    else if(models[i].from->ID == to && models[i].to->ID == from){
      return true;
    }
  }

  //check that combo is not in "invalid legs"
  for(int i = 0; i < invalid_legs.size(); i++){
    //itterate through each model
    //check from --> to
    if(invalid_legs[i].from->ID == from && invalid_legs[i].to->ID == to){
      return true;
    }
    //check to --> from
    else if(invalid_legs[i].from->ID == to && invalid_legs[i].to->ID == from){
      return true;
    }
  }
  return false;
}

bool Network::combo_exists(string from, string to, vector<Model> &models){
  //If from and to are the same returns true b/c it doesn't exist but should not be made
  if (from == to){
    return true;
  }

  //check all models for combo existance
  for(int i = 0; i < models.size(); i++){
    //itterate through each model
    //check from --> to

    if(models[i].from->ID == from && models[i].to->ID == to){
      return true;
    }
    //check to --> from
    else if(models[i].from->ID == to && models[i].to->ID == from){
      return true;
    }
  }

  //check that combo is not in "invalid legs"
  for(int i = 0; i < invalid_legs.size(); i++){
    //itterate through each model
    //check from --> to
    if(invalid_legs[i].from->ID == from && invalid_legs[i].to->ID == to){
      return true;
    }
    //check to --> from
    else if(invalid_legs[i].from->ID == to && invalid_legs[i].to->ID == from){
      return true;
    }
  }
  return false;
}

void Network::output_error_legs(){

  vector<ErrorFeature> errors = error_features();

  
  for( int i = 0; i < errors.size(); i++){
    //output model information
    errors[i].output();
  }
}

bool Network::combo_exists(vector<vector<Model>> &curr, vector<Model> &b){
  for (int y = 0; y < curr.size(); y++) { 
    bool test = true;


      if( curr[y].size() != b.size()){
        //if different lengths then clearly not the same set
        test = false;
      }

      //check all models for combo existance
      for(int i = 0; i < curr[y].size(); i++){
        bool exists = false;

        for (int j = 0; j < b.size(); i++){

          if(curr[y][i].from->ID == b[j].from->ID && curr[y][i].to->ID == b[j].to->ID){
            exists = true;
          }
          //check to --> from
          if(curr[y][i].from->ID == b[j].to->ID && curr[y][i].to->ID == b[j].from->ID){
            exists = true;
          }
        }

        if (!exists){
          //if this was false then the model checked from a did not exist in b and therefore the two sets are different
          test = false;
        }
      }  
      if(test){
        return true;
      }    
    }
  //all models proved to not be the same as the new one
  return false;
}

bool Network::same(vector<Model> &an, vector<Model> &b){

  if( an.size() != b.size()){
    //if different lengths then clearly not the same set
    return false;
  }

   //check all models for combo existance
  for(int i = 0; i < an.size(); i++){
    bool exists = false;

    for (int j = 0; j < b.size(); i++){

      if(an[i].from->ID == b[j].from->ID && an[i].to->ID == b[j].to->ID){
        exists = true;
      }
      //check to --> from
      if(an[i].from->ID == b[j].to->ID && an[i].to->ID == b[j].from->ID){
        exists = true;
      }
    }

    if (!exists){
      //if this was false then the model checked from a did not exist in b and therefore the two sets are different
      return false;
    }
  }

  //therefore all same size and same models
  return true;
  
}

vector<Model> Network::generate_unknown_legs(){
  vector<Model> legs;
  string line;
  //skip first fake point
  for(int i = 1; i < unknowns.size(); i++){
    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it

      if( !combo_exists(unknowns[i].ID, unknowns[j].ID, legs) ){
        //create string to initialize the model
        line = unknowns[i].ID + "   " + unknowns[j].ID + "   -1";

        //create model
        legs.emplace_back(1, line, knowns, unknowns);
      }
    }
  }

  return legs;
}

void Network::quick_print(vector<Model> &m, int n){
  if(n == -1){
    cout << "_____Printing Set______" << endl;
  }
  else{
    cout << "_____Printing Set " << n << "______" << endl;
  }

  for( int i = 0; i<m.size(); i++){
    m[i].quick_print();
  }
}

bool Network::passes(double d, double c, double mult){
  vector<ErrorFeature> errors = error_features(mult);

  double meas, error;

  //this turns to false if one measurement does not pass
  bool all_passed = true;
  for( int i = 0; i < errors.size(); i++){

    //estimated distance of that model
    meas = errors[i].model->meas_approx();

    //acceptable error for that semimajor-axis
    error = meas * d + c;

    //if true then within the acceptable range
    if(errors[i].a < error){
      cout << "Required Error: " << error << endl;
      cout << "____Model Passed____" << endl;
      errors[i].output();
    }

    else{
      all_passed = false;
      cout << "____Model Failed____" << endl;
      cout << "Required Error: " << error << endl;
      errors[i].output();
    }
  }
  return all_passed;
}

vector<string> Network::combos(){
  vector<Model> legs;
  string line;
  //skip first fake point
  for(int i = 1; i < unknowns.size(); i++){
    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it

      if( !combo_exists(unknowns[i].ID, unknowns[j].ID, legs) ){
        //create string to initialize the model
        line = unknowns[i].ID + "   " + unknowns[j].ID + "   -1";

        //create model
        legs.emplace_back(1, line, knowns, unknowns);
      }
    }
  }

  vector<string> arr;

  for(int i = 0; i < legs.size(); i++){
    arr.push_back(legs[i].from->ID + "   " + legs[i].to->ID + "   -1");
  }

  return arr;
}

double Network::average_a(double multiplier){
  vector<ErrorFeature> errors = error_features(multiplier);

  double sum = 0;
  for(int i = 0; i < errors.size(); i++){
    sum = sum + errors[i].a;
  }
  
  return sum / errors.size();
}

  vector<Model>  Network::max_legs(){
  vector<Model> models;
  //this is used for declaring each model
  string line;
  //skip first fake point
  for(int i = 1; i < knowns.size(); i++){

    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it
      if( !combo_exists(knowns[i].ID, unknowns[j].ID) ){
        //create string to initialize the model
        
        line = knowns[i].ID + "   " + unknowns[j].ID + "   -1";
        //create model
        models.emplace_back(1, line, knowns, unknowns);
      }
    }
  }

  //skip first fake point
  for(int i = 1; i < unknowns.size(); i++){
    //create unique combo with unknowns
    for(int j = 1; j < unknowns.size(); j++){
      //if combo does not exist then we can make a model of it

      if( !combo_exists(unknowns[i].ID, unknowns[j].ID) ){

        //create string to initialize the model
        line = unknowns[i].ID + "   " + unknowns[j].ID + "   -1";

        //create model
        models.emplace_back(1, line, knowns, unknowns);
      }
    }

    //known combos were already tested
  }
  return models;
}

void Network::output_grids(){
  for(int i=0; i<models.size();i++){
    models[i].Dg();
  }
}

void Network::establish_scales(string line){
        //Update the filetype
        //If it switches value then another line needs to be read
        string arr[7];
        int i = 0;

        //read in each of the seven elements
        stringstream ssin(line);
        while (ssin.good() && i < 7){
          
            ssin >> arr[i];
            ++i;
        }

        //this will be the id of the point we're setting the scale to
        string ID = arr[0];
        //assign to correct point (check unknowns)
        for(int i=1; i<unknowns.size(); i++){
          if(unknowns[i].ID == ID){
            double lat = degrees(stold(arr[1]), stold(arr[2]), stold(arr[3]));
            double lon = degrees(stold(arr[4]), stold(arr[5]), stold(arr[6]));
            unknowns[i].scale(lat, lon);
          }
        }
        //assign to correct point (check knowns)
        for(int i=1; i<knowns.size(); i++){
          if(knowns[i].ID == ID){
            double lat = degrees(stold(arr[1]), stold(arr[2]), stold(arr[3]));
            double lon = degrees(stold(arr[4]), stold(arr[5]), stold(arr[6]));
            knowns[i].scale(lat, lon);
          }
        }

  return;
}

void Network::model_scale(string line){
  string to;
  string from;
  //Update the filetype
  //If it switches value then another line needs to be read
  string arr[8];
  int i = 0;

  //read in each of the seven elements
  stringstream ssin(line);
  while (ssin.good() && i < 8){
    ssin >> arr[i];
    ++i;
  }

  from = arr[0];
  to = arr[1];

  //check all models for combo existance
  for(int i = 0; i < models.size(); i++){
    //itterate through each model
    //check from --> to

    if(models[i].from->ID == from && models[i].to->ID == to){
      //cout << "TwoB" << endl;
      double lat = degrees(stold(arr[2]), stold(arr[3]), stold(arr[4]));
      double lon = degrees(stold(arr[5]), stold(arr[6]), stold(arr[7]));
      models[i].middle_scale(lat, lon);
    }
    //check to --> from
    else if(models[i].from->ID == to && models[i].to->ID == from){
      double lat = degrees(stold(arr[2]), stold(arr[3]), stold(arr[4]));
      double lon = degrees(stold(arr[5]), stold(arr[6]), stold(arr[7]));
      models[i].middle_scale(lat, lon);
    }
  }

  //check that combo is not in "invalid legs"
  for(int i = 0; i < invalid_legs.size(); i++){
    //itterate through each model
    //check from --> to
    if(invalid_legs[i].from->ID == from && invalid_legs[i].to->ID == to){
      double lat = degrees(stold(arr[2]), stold(arr[3]), stold(arr[4]));
      double lon = degrees(stold(arr[5]), stold(arr[6]), stold(arr[7]));
      models[i].middle_scale(lat, lon);
    }
    //check to --> from
    else if(invalid_legs[i].from->ID == to && invalid_legs[i].to->ID == from){
      double lat = degrees(stold(arr[2]), stold(arr[3]), stold(arr[4]));
      double lon = degrees(stold(arr[5]), stold(arr[6]), stold(arr[7]));
      models[i].middle_scale(lat, lon);
    }
  }
}

MatrixXd Network::S(){
  MatrixXd cl = Cl();
  MatrixXd A1 = A();
  MatrixXd Cs = (A1.transpose()*cl.inverse()*A1).inverse();

  return -(Cs*A1.transpose()*cl.inverse()*W());
}

MatrixXd Network::R(){
  return A()*S()+W();
}

MatrixXd Network::a_post(){

  MatrixXd a(1,1);
  a(0,0) = (1.0/(n-u));
  
  return a*(R().transpose()*Cl().inverse()*R());
}

MatrixXd Network::X_0(){
  int a = unknowns.size()-1;
  MatrixXd X(a*2, 1);

  int placeholder = 0;

  for(int i = 1; i < unknowns.size(); i++){
    X(placeholder, 0) = unknowns[i].E;

    placeholder++;
    X(placeholder, 0) = unknowns[i].N;

    placeholder++;
  }

  return X;
}

void Network::reduce(){
  cout << "Reduced Measurements" << endl;
  for(int i = 0; i< models.size(); i++){
    double dg = models[i].Dg();
    models[i].meas = dg;
    cout << dg << endl;
  }
}