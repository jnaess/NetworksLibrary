#include "ErrorFeature.h"

ErrorFeature::ErrorFeature(){
  return;
}

ErrorFeature::ErrorFeature(double e, double n, double d, Model *mode, double mult){
  A(e, n, d, mult);
  B(e, n, d, mult);
  Bearing(e, n, d);

  //initialize pointers incase this information is useful in the future
  model = mode;
  model->output();
}

void ErrorFeature::A(double e, double n, double d, double multiplier){
  a = multiplier*sqrt(fabs(0.5*(e*e + n*n + sqrt((e*e - n*n)*(e*e - n*n) + 4 * (d*d)))));
}

void ErrorFeature::B(double e, double n, double d, double multiplier){
  b =  multiplier*sqrt(fabs(0.5*(e*e + n*n - sqrt((e*e - n*n)*(e*e - n*n) + 4 * (d*d)))));
}

void ErrorFeature::Bearing(double e, double n, double d){
  bearing = atan(2 * d / ( e*e - n*n));
}

void ErrorFeature::output(){
  //output model information
  model->quick_print();
  cout << "a: " << a << endl;
  cout << "b: " << b << endl;
  cout << "Bearing: " << bearing << endl;
}