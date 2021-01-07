#include "Point.h"

Point::Point(){
  ID = "null";
  E = -1;
  N = -1;
}

Point::Point(string str){
    string arr[7];
    int i = 0;

    //splits the line from spaces into an array of three
    stringstream ssin(str);
    while (ssin.good() && i < 7){
        ssin >> arr[i];
        ++i;
    }
  //regular initialization
  //Initiate all values
    ID = arr[0];
    //convert string to a long double
    E = stold(arr[1]);
    N = stold(arr[2]);

    delta_h = stold(arr[3]);
    H = stold(arr[4]);
    N_g = stold(arr[5]);
    R = stold(arr[6]);

    // we can now find a few important values
    h = N_g + H;
    h_prime = h + delta_h;

  //check that it is not one of the default contructed points
  if((stold(arr[6]) == -1 && stold(arr[1]) != -1)){
    ID = arr[0];
    delta_h = stold(arr[1]);
    h = stold(arr[2]);
    E = stold(arr[3]);
    N = stold(arr[4]);
    h_prime = h + delta_h;
  }
  //type two initialization
}

void Point::output(){
  cout << "Point: " << ID;
  cout << "   [Easting: " << E;
  cout << ", Northing: " << N << "]" << endl;

}

string Point::point_ID(){
  return ID;
}

void Point::scale(double lat, double lon){
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
  //double ellipsoid = datum ? datum.ellipsoid : LatLonEllipsoidal.ellipsoids.WGS84;
  
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
  //double scale = Number(k.toFixed(12));

  //assigns the scale value to the point object
  Scale = k;
  //Assign easting 
  E = x;
  //Assign northing
  N = y;

  double h = lat>=0 ? 'N' : 'S'; // hemisphere

  //return new Utm(zone, h, x, y, this.datum, convergence, scale, !!zoneOverride);

  //also calculate the radius
  calc_radius(lat);

  return;
}

void Point::calc_radius(double lat){
  //cout << "Calculating Radius of point " << ID << endl;
  
  double B = radians(lat);//convert to radians
  double r1 = 6378137;
  double r2 = 6356752.314140347;
  double Radius = sqrt(((r1*r1 * cos(B))*(r1*r1 * cos(B)) + (r2*r2 * sin(B))*(r2*r2 * sin(B)))/
                  ((r1 * cos(B))*(r1 * cos(B)) + (r2 * sin(B))*(r2 * sin(B))));

  //set point's radius variable
  R = Radius;
  return;
}