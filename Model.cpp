#include "Model.h"

Model::Model(){
  
}

Model::Model(int t, string line, vector<Point> &knowns, vector<Point> &unknowns){
  //If it is a linear type
  if(t == 1){
    //reads in the line to find Point ID's and measurement nubmer
    string arr[3];
    int i = 0;
    //splits the line from spaces into an array of three
      stringstream ssin(line);
      while (ssin.good() && i < 3){
          ssin >> arr[i];
          ++i;
      }

    //ID of the point from
    string ID_From = arr[0];
    //ID of the point to
    string ID_To = arr[1];

    //Initiates a few object variables
    //Double of distance measurement
    meas = stold(arr[2]);
    //type initiated to distance b/c of only 3 parameters
    type = "distance";

    //placeholder index
    int j;

    //determine From placeholder
    if(-1 !=find_placeholder(ID_From, knowns)){
      //The point was found in the list of knowns
      j = find_placeholder(ID_From, knowns);
      from = &knowns[j];
    }
    else{
      //Then the point must be an unknown point
      j = find_placeholder(ID_From, unknowns);
      from = &unknowns[j];
    }

    //determine From placeholder
    if(-1 !=find_placeholder(ID_To, knowns)){
      //The point was found in the list of knowns
      j = find_placeholder(ID_To, knowns);
      to = &knowns[j];
    }
    else{
      //Then the point must be an unknown point
      j = find_placeholder(ID_To, unknowns);
      to = &unknowns[j];
    }

    //itinitalize standard deviation base off of these metrics
    stddev = .004 + meas_approx()*2/1000000;
  }

  //Angle Model
  else if(t == 2){
        //reads in the line to find Point ID's and measurement nubmer
    string arr[6];
    int i = 0;
    //splits the line from spaces into an array of three
      stringstream ssin(line);
      while (ssin.good() && i < 6){
          ssin >> arr[i];
          ++i;
      }

    //ID of the point at
    string ID_At = arr[0];
    //ID of the point from
    string ID_From = arr[1];
    //ID of the point to
    string ID_To = arr[2];


    //Initiates a few object variables
    //Double of distance measurement
    meas = degrees(stold(arr[3]),stold(arr[4]),stold(arr[5]));
    //type initiated to distance b/c of only 3 parameters
    type = "angle";

    //placeholder index
    int j;

    //determine From placeholder
    if(-1 !=find_placeholder(ID_At, knowns)){
      //The point was found in the list of knowns
      j = find_placeholder(ID_At, knowns);
      at = &knowns[j];
    }
    else{
      //Then the point must be an unknown point
      j = find_placeholder(ID_At, unknowns);
      at = &unknowns[j];
    }

    //determine From placeholder
    if(-1 !=find_placeholder(ID_From, knowns)){
      //The point was found in the list of knowns
      j = find_placeholder(ID_From, knowns);
      from = &knowns[j];
    }
    else{
      //Then the point must be an unknown point
      j = find_placeholder(ID_From, unknowns);
      from = &unknowns[j];
    }

    //determine From placeholder
    if(-1 !=find_placeholder(ID_To, knowns)){
      //The point was found in the list of knowns
      j = find_placeholder(ID_To, knowns);
      to = &knowns[j];
    }
    else{
      //Then the point must be an unknown point
      j = find_placeholder(ID_To, unknowns);
      to = &unknowns[j];
    }
  }
}

int Model::find_placeholder(string ID, vector<Point> &points){
  for(int i=0; i<points.size();i++){
    if(points[i].ID == ID)
      return i;
  }
  return -1;
}

void Model::output(){
  //Picks the type of model to be outputting
  if(type == "distance"){
    output_distance();
  }
  else if(type == "angle"){
    output_angle();
  }
  //cout << "Neither" << endl;
}

void Model::output_distance(){
  cout << "___Distance Model___" << endl;

  //From point
  cout << "From ";
  from->output();

  //To point
  cout << "To ";
  to->output();

  //measurement
  cout << "Distance of: " << meas << endl;

  cout << "Combined Scale Factor: " << scale()*ke() << endl;
}

void Model::output_angle(){
  //Add angular features
  cout << "___Angle Model___" << endl;
  
  //From point
  cout << "At ";
  at->output();

  //From point
  cout << "From ";
  from->output();

  //To point
  cout << "To ";
  to->output();

  //measurement
  cout << "Angle of: " << dms(meas)[0] << "° " <<  dms(meas)[1] << "' " << dms(meas)[2] << "\"" <<endl << endl;
}

double Model::distance_from(string axis){
  if(axis == "N"){
    //Partial of Northing
    
    return ( to->N - from->N)/meas_approx();
  }
  else{
    //Partial of easting
    return ( to->E-from->E )/meas_approx();
  }
}

double Model::distance_to(string axis){
  if(axis == "N"){
    //Partial of Northing
    return (from->N - to->N  )/meas_approx();
  }
  else{
    //Partial of easting
    return ( from->E - to->E)/meas_approx();
  }
}

double Model::angle_from(string axis){
  if(axis == "N"){
    //Partial of Northing
    return ( at->E - from->E )/(meas_approx()*meas_approx());
    
  }
  else{
    //Partial of easting
    return ( from->N - at->N  )/(meas_approx()*meas_approx());
  }
}

double Model::angle_to(string axis){
  if(axis == "N"){
    //Partial of Northing
    type = "angle1";
    double temp = ( to->E - at->E )/(meas_approx()*meas_approx());
    type = "angle";

    return temp;
    
  }
  else{

    //Partial of easting
    type = "angle1";
    double temp = ( at->N - to->N )/(meas_approx()*meas_approx());
    type = "angle";
    
    return temp;
    
  }
}

double Model::meas_approx(){
  //Returns a length for the distance model
  //Different for angle model
  if (type == "distance"){
  return sqrt((to->E - from->E)*(to->E - from->E)
              +(to->N - from->N)*(to->N - from->N));
  }

  else if(type == "angle"){
  return sqrt((at->E - from->E)*(at->E - from->E)
          +(at->N - from->N)*(at->N - from->N));
  }

  //enter new one here
  else if(type == "angle1"){
  return sqrt((at->E - to->E)*(at->E - to->E)
          +(at->N - to->N)*(at->N - to->N));
  }

  else return -1;
}

double Model::part(string ID, string axis){
  //check the model
  //checking for distance model
  if( type == "distance"){
    //check if its a the 'to' point
    if(ID == to->point_ID()){
      //Find Easting or Northing Partial of 'to'
      return distance_to(axis);
    }
    else if(ID == from->point_ID()){
      //partial of 'from'
      return distance_from(axis);
    }
  }

  //check the model
  //checking for angle model
  else if (type == "angle"){
      //check if its a the 'to' point
      if(ID == to->point_ID()){
        //Find Easting or Northing Partial of 'to'
        return angle_to(axis);
      }
      else if(ID == from->point_ID()){
        //partial of 'from'
        return angle_from(axis);
      }
  }

  //in this case the point did not exsist in the linearized model and therefore the partial derivative would be 0
  return 0;
}

double Model::meas_estimate(){
  //return meas_approx if model is distance
  if( type == "distance")
    return meas_approx();
    
  else if (type == "angle"){

    return degrees(
      (atan2(from->E - at->E,from->N - at->N))
    - (atan2(to->E - at->E,to->N - at->N))
    );
  }
  //the model was not assigned
  return -1;
}

void Model::set_stddev(double std, double ppm){
  if( type == "distance"){
    stddev = std + ppm*meas_estimate()/1000000;
  }
  else stddev = 0;
}

void Model::quick_print(){
  cout << from->ID << " --> " << to->ID << endl;
}

double Model::D1(){
  return sqrt(abs(D3()*D3()*(1+from->h/from->R)*(1+to->h/to->R)
          +(from->h-to->h)*(from->h-to->h)));
}

double Model::D2(){
  

  double R = (from->R+to->R)/2;
  return 2*R*asin(D3()/(2*R));
}

double Model::D3(){
  return sqrt(abs((meas * meas - ((from->h_prime - to->h_prime) * (from->h_prime - to->h_prime)))/
              ((1 + from->h_prime / from->R) * (1 + to->h_prime / to->R))));
}

double Model::scale(){
  if(meas < 1000){
    return from->Scale;
  }

  else if(meas > 1000 && meas < 4000){
    return (from->Scale + to->Scale)/2;
  }

  else {
    //calc middle point
    //if middle point is -1 then it needs to be calculated
    if(middle_k==-1){
      double E = from->E - (from->E - to->E)/2;
      double N = from->N - (from->N - to->N)/2;
      cout << "Please Enter the" << endl;
      cout << "Easting: " << E << endl;
      cout << "Northing: " << setprecision(9) << N << endl;

      double lonD;
      double lonM;
      double lonS;
      double latD;
      double latM;
      double latS;

      cout << "Enter the latitude: ";
      cin >> latD >> latM >> latS;
      cout << endl << "Enter the longitude: ";
      cin >> lonD >> lonM >> lonS;

      middle_k = middle_scale(degrees(latD,latM,latS),degrees(lonD,lonM,lonS));
      cout << endl << "The middle scale is: " << middle_k << endl;
      
      double k = (from->Scale + 4*middle_k + to->Scale)/6;
      cout << "The total scale is: " << middle_k << endl;
      return middle_k;
    }
    //otherwise k has already been genrated
    return (from->Scale + 4*middle_k + to->Scale)/6;
  }
  return -1;
}

double Model::Dg(){
  double dg = D2()*scale();

  return dg;
}

double Model::ke(){
  return D2() / meas;
}

double Model::middle_scale(double lat, double lon){
  if (!(-80<=lat && lat<=84)){
    cout << "outside limits";
  }

  double falseEasting = 500e3, falseNorthing = 10000e3;

  int zone = floor((lon + 180)/6) + 1; // longitudinal zone
  double λ_0 = radians(((zone-1)*6 - 180 + 3)); // longitude of central meridian

  // ---- handle Norway/Svalbard exceptions
  // grid zones are 8° tall; 0°N is offset 10 into latitude bands array
  char mgrsLatBands[] = {'C','D','E','F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','X'}; // X is repeated for 80-84°N
  char latBand = mgrsLatBands[int(floor(lat/8+10))];

  // adjust zone & central meridian for Norway
  if (zone==31 && latBand=='V' && lon>= 3) { zone++; λ_0 += radians(6); }
  // adjust zone & central meridian for Svalbard
  if (zone==32 && latBand=='X' && lon<  9) { zone--; λ_0 -= radians(6); }
  if (zone==32 && latBand=='X' && lon>= 9) { zone++; λ_0 += radians(6); }
  if (zone==34 && latBand=='X' && lon< 21) { zone--; λ_0 -= radians(6); }
  if (zone==34 && latBand=='X' && lon>=21) { zone++; λ_0 += radians(6); }
  if (zone==36 && latBand=='X' && lon< 33) { zone--; λ_0 -= radians(6); }
  if (zone==36 && latBand=='X' && lon>=33) { zone++; λ_0 += radians(6); }

  double φ = radians(lat);      // latitude ± from equator
  double λ = radians(lon) - λ_0; // longitude ± from central meridian

  // allow alternative ellipsoid to be specified
  
  //GRS 80
  //source: https://en.wikipedia.org/wiki/Geodetic_Reference_System_1980
  double a = 6378137;
  double f = 0.003352810681183637418;

  double k0 = 0.9996; // UTM scale on the central meridian

  // ---- easting, northing: Karney 2011 Eq 7-14, 29, 35:

  double e = sqrt(f*(2-f)); // eccentricity
  double n = f / (2 - f);        // 3rd flattening
  double n2 = n*n, n3 = n*n2, n4 = n*n3, n5 = n*n4, n6 = n*n5;

  double cosλ = cos(λ), sinλ = sin(λ), tanλ = tan(λ);

  double τ = tan(φ); // τ ≡ tanφ, τʹ ≡ tanφʹ; prime (ʹ) indicates angles on the conformal sphere
  double σ = sinh(e*atanh(e*τ/sqrt(1+τ*τ)));

  double τʹ = τ*sqrt(1+σ*σ) - σ*sqrt(1+τ*τ);

  double ξʹ = atan2(τʹ, cosλ);
  double ηʹ = asinh(sinλ / sqrt(τʹ*τʹ + cosλ*cosλ));

  double A = a/(1+n) * (1 + 1/4*n2 + 1/64*n4 + 1/256*n6); // 2πA is the circumference of a meridian

  double α [7]; // note α is one-based array (6th order Krüger expressions)
  α[1] =           1.0 /2*n - 2.0 /3*n2 + 5.0 /16*n3 +   41.0 /180*n4 -     127.0 /288*n5 +      7891.0 /37800*n6;
  α[2] =                 13.0 /48*n2 -  3.0/5*n3 + 557.0/1440*n4 +     281.0/630*n5 - 1983433.0/1935360*n6;
  α[3] =                          61.0/240*n3 -  103.0/140*n4 + 15061.0/26880*n5 +   167603.0/181440*n6;
  α[4] =                                  49561.0/161280*n4 -     179.0/168*n5 + 6601661.0/7257600*n6;
  α[5] =                                                    34729.0/80640*n5 - 3418889.0/1995840*n6;
  α[6] =                                                                 212378941.0/319334400*n6 ;

  double ξ = ξʹ;
  for (int j=1; j<=6; j++){
    ξ += α[j] * sin(2*j*ξʹ) * cosh(2*j*ηʹ);
  }

  double η = ηʹ;
  for (int j=1; j<=6; j++){
    η += α[j] * cos(2*j*ξʹ) * sinh(2*j*ηʹ);
  } 

  double x = k0 * A * η;
  double y = k0 * A * ξ;

  // ---- convergence: Karney 2011 Eq 23, 24

  double pʹ = 1;
  for (int j=1; j<=6; j++){
    pʹ += 2*j*α[j] * cos(2*j*ξʹ) * cosh(2*j*ηʹ);
  }
  double qʹ = 0;
  for (int j=1; j<=6; j++){
    qʹ += 2*j*α[j] * sin(2*j*ξʹ) * sinh(2*j*ηʹ);
  }

  double γʹ = atan(τʹ / sqrt(1+τʹ*τʹ)*tanλ);
  double γʺ = atan2(qʹ, pʹ);

  double γ = γʹ + γʺ;

        // ---- scale: Karney 2011 Eq 25

  double sinφ = sin(φ);
  double kʹ = sqrt(1 - e*e*sinφ*sinφ) * sqrt(1 + τ*τ) / sqrt(τʹ*τʹ + cosλ*cosλ);
  double kʺ = A / a * sqrt(pʹ*pʹ + qʹ*qʹ);

  double k = k0 * kʹ * kʺ;

        // ------------

        // shift x/y to false origins
  x = x + falseEasting;             // make x relative to false easting
  if (y < 0){ 
    y = y + falseNorthing;
   } // make y in southern hemisphere relative to false northing

        // round to reasonable precision
  //JE: round to 9 decimal places
  double convergence = degrees(γ);


  double h = lat>=0 ? 'N' : 'S'; // hemisphere

  //JE: At this point x and y should be northing and easting I think

  middle_k = k;
  return k;
}

void Model::scale_generating_output(){
  cout << setprecision(9);
  
  cout << "________________________________________" << endl;
  quick_print();

  //from point
  cout << "____From Point: " << from->ID << endl;
  cout << "E: " << from->E << "\t\tN: " << from->N << endl;
  cout << "delta_h: " << from->delta_h << "\t\tH: " << from->H << endl;
  cout << "N_g: " << from->N_g << "\t\t\t\th_prime: " << from->h_prime << endl;
  cout << "R: " << from->R << "\t\th: " << from->h << endl;
  cout << "Scale: " << from->Scale << endl;

  //to point
  cout << "____To Point: " << to->ID << endl;
  cout << "E: " << to->E << "\t\tN: " << to->N << endl;
  cout << "delta_h: " << to->delta_h << "\t\tH: " << to->H << endl;
  cout << "N_g: " << to->N_g << "\t\t\t\th_prime: " << to->h_prime << endl;
  cout << "R: " << to->R << "\t\th: " << to->h << endl;
  cout << "Scale: " << to->Scale << endl;

  //Grid Distances
  cout << "meas: " << meas << endl;
  cout << "_____D1()____: " << D1() << endl;
  cout << "_____D2()____: " << D2() << endl;
  cout << "_____D3()____: " << D3() << endl;
  cout << "_____Dg()____: " << Dg() << endl;

  //scales
  cout << "ke(): " << ke() << endl;
  cout << "scale(): " << scale() << endl;

  //additional end for formatting
  cout << "________________________________________" << endl << endl;
}